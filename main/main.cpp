// clang-format off
#include <cstddef>
#include <glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "loadobj.hpp"
#include "shapes.hpp"

#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/vec4.hpp"

#include <memory>
#include <stdexcept>
#include <typeinfo>

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ostream>
#include <fstream>

#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"
#include "../support/error.hpp"
#include "../support/program.hpp"

#include "../vmlib/mat44.hpp"
#include "../vmlib/vec4.hpp"

#include "defaults.hpp"
#include "spaceship.hpp"
#include "texture.hpp"

#include "particle_system.hpp"

// Vectors to hold render times for benchmarking
std::vector<double> fullRenderTime;
std::vector<double> customRenderTime;
std::vector<double> otherRenderTime;

namespace {
constexpr char const *kWindowTitle = "COMP3811 - CW2";

constexpr float kPi_ = 3.1415926f;

constexpr float kMovementPerSecond_ = 5.f;  // units per second
constexpr float kMouseSensitivity_ = 0.01f; // radians per pixel

struct State_ {
  ShaderProgram *prog;

  struct CamCtrl_ {
    bool cameraActive;
    bool moveForward, moveBackward, moveLeft, moveRight;

    float phi, theta;
    // float radius;

    float lastX, lastY;

    float x, y, z;
    float speed = 1.f;
  } camControl;
  CamCtrl_ mainTrackingCameraDynamic;
  CamCtrl_ mainTrackingCameraStatic;

  CamCtrl_ splitCam;
  CamCtrl_ splitTrackingCameraDynamic;
  CamCtrl_ splitTrackingCameraStatic;

  bool splitScreenActive = 0;
  bool activeScreen = 0; // false/0 = screen 0; true/1 = screen 1


  unsigned int mainCameraType = 0; // 0 = normal; 1 = tracking camera dynamic; 2 = tracking camera static
  unsigned int splitCameraType = 0; // same for this

  struct Animation_ {
    bool animated;
    float time;
  } animation;
};

void glfw_callback_error_(int, char const *);

void glfw_callback_key_(GLFWwindow *, int, int, int, int);
void glfw_callback_motion_(GLFWwindow *, double, double);
void mouse_click_callback_(GLFWwindow* window, int button, int action, int mods);

struct GLFWCleanupHelper {
  ~GLFWCleanupHelper();
};
struct GLFWWindowDeleter {
  ~GLFWWindowDeleter();
  GLFWwindow *window;
};
} // namespace

int main() try {
  // Initialize GLFW
  if (GLFW_TRUE != glfwInit()) {
    char const *msg = nullptr;
    int ecode = glfwGetError(&msg);
    throw Error("glfwInit() failed with '%s' (%d)", msg, ecode);
  }

  // Ensure that we call glfwTerminate() at the end of the program.
  GLFWCleanupHelper cleanupHelper;

  // Configure GLFW and create window
  glfwSetErrorCallback(&glfw_callback_error_);

  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  // glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_DEPTH_BITS, 24);

#if !defined(NDEBUG)
  // When building in debug mode, request an OpenGL debug context. This
  // enables additional debugging features. However, this can carry extra
  // overheads. We therefore do not do this for release builds.
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // ~ !NDEBUG

  GLFWwindow *window =
      glfwCreateWindow(1280, 720, kWindowTitle, nullptr, nullptr);

  if (!window) {
    char const *msg = nullptr;
    int ecode = glfwGetError(&msg);
    throw Error("glfwCreateWindow() failed with '%s' (%d)", msg, ecode);
  }

  GLFWWindowDeleter windowDeleter{window};

  State_ state{};

  // Set up event handling
  // TODO: Additional event handling setup
  glfwSetWindowUserPointer(window, &state);

  glfwSetKeyCallback(window, &glfw_callback_key_);
  glfwSetCursorPosCallback(window, &glfw_callback_motion_);
  glfwSetMouseButtonCallback(window, &mouse_click_callback_);

  // Set up drawing stuff
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // V-Sync is on.

  // Initialize GLAD
  // This will load the OpenGL API. We mustn't make any OpenGL calls before
  // this!
  if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
    throw Error("gladLoaDGLLoader() failed - cannot load GL API!");

  std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
  std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
  std::printf("VERSION %s\n", glGetString(GL_VERSION));
  std::printf("SHADING_LANGUAGE_VERSION %s\n",
              glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Ddebug output
#if !defined(NDEBUG)
  setup_gl_debug_output();
#endif // ~ !NDEBUG

  // Global GL state
  OGL_CHECKPOINT_ALWAYS();

  glEnable(GL_FRAMEBUFFER_SRGB);

  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

  OGL_CHECKPOINT_ALWAYS();

  // Get actual framebuffer size.
  // This can be different from the window size, as standard window
  // decorations (title bar, borders, ...) may be included in the window size
  // but not be part of the drawable surface area.
  int iwidth, iheight;
  glfwGetFramebufferSize(window, &iwidth, &iheight);

  glViewport(0, 0, iwidth, iheight);

  // Set shader programs
  ShaderProgram prog({{GL_VERTEX_SHADER, "assets/default.vert"},
                      {GL_FRAGMENT_SHADER, "assets/default.frag"}});

  ShaderProgram ui({{GL_VERTEX_SHADER, "assets/2dshader.vert"},
                      {GL_FRAGMENT_SHADER, "assets/2dshader.frag"}});

  state.prog = &prog;

  auto last = Clock::now();

  // Load objects to be rendered
  std::vector<GLuint> vaos, ui_vaos;
  std::vector<std::size_t> vertexCounts, vertexCountsUI;
  std::vector<GLuint> textures, ui_texture;

  GLuint tex = load_texture_2d("assets/L4343A-4k.jpeg");

  auto map = load_wavefront_obj("assets/parlahti.obj");
  GLuint vao = create_vao(map);
  vaos.push_back(vao);
  vertexCounts.push_back(map.positions.size());
  textures.push_back(tex);

  // Load in launchpad 1 and transform each vertex to the location we want
  auto launchhpad = load_wavefront_obj("assets/landingpad.obj");
  for (auto &p : launchhpad.positions) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = make_translation(Vec3f{-10.f, -0.97f, 15.f}) * p4;
    t /= t.w;

    Vec3f pTransformed{t.x, t.y, t.z};
    p = pTransformed;
  }

  // Create VAO
  vao = create_vao(launchhpad);
  vaos.push_back(vao);
  vertexCounts.push_back(launchhpad.positions.size());
  textures.push_back(0);

  // Load in launchpad 1 and transform each vertex to the location we want
  launchhpad = load_wavefront_obj("assets/landingpad.obj");
  for (auto &p : launchhpad.positions) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = make_translation(Vec3f{-50.f, -0.97f, 20.f}) * p4;
    t /= t.w;

    Vec3f pTransformed{t.x, t.y, t.z};
    p = pTransformed;
  }

  // Create VAO
  vao = create_vao(launchhpad);
  vaos.push_back(vao);
  vertexCounts.push_back(launchhpad.positions.size());
  textures.push_back(0);

  // Creating spaceship
  Spaceship spaceship(10, kIdentity44f *
                              make_translation({-10.f, -0.9f, 15.f}) *
                              make_scaling(0.1f, 0.1f, 0.1f));


  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Initialise particle and particleSystem
  ParticleInit particle;
  ParticleSystem particleSystem;

  particle.ColorBegin = {254.0f / 255.0f, 212.0f / 255.0f, 123.0f / 255.0f,
                         1.0f};
  particle.ColorEnd = {254.0f / 255.0f, 109.0f / 255.0f, 41.0f / 255.0f, 1.0f};
  particle.SizeBegin = 1.f, particle.SizeVariation = 0.5f,
  particle.SizeEnd = 0.0f;
  particle.Velocity = {0.f, -1.f, 0.f};
  particle.LifeTime = 1.0f;
  particle.VelocityVariation = {0.f, 0.f, 0.f};
  particle.Position = {-10.0f, -0.9f, 15.0f};
  particle.PositionVariation = {0.1f, 0.1f, 0.1f};

  std::chrono::steady_clock::time_point prevTime =
      std::chrono::steady_clock::now();

  SimpleMeshData rectangle, button_one, button_two;

  // 2D UI Boxes
  GLuint ui_vao = create_rectangle({ -0.8f, 0.8f, 0.0f }, { -0.6f, 0.8f, 0.0f }, { -0.8f, 0.6f, 0.0f }, { -0.6f, 0.6f, 0.0f }, rectangle);
  ui_vaos.push_back(ui_vao);
  vertexCountsUI.push_back(rectangle.positions.size());
  ui_vao = create_rectangle({ 0.4f, -0.6f, 0.0f }, { 0.1f, -0.6f, 0.0f }, { 0.4f, -0.8f, 0.0f }, { 0.1f, -0.8f, 0.0f }, button_one);
  ui_vaos.push_back(ui_vao);
  vertexCountsUI.push_back(button_one.positions.size());
  ui_vao = create_rectangle({ -0.4f, -0.6f, 0.0f }, { -0.1f, -0.6f, 0.0f }, { -0.4f, -0.8f, 0.0f }, { -0.1f, -0.8f, 0.0f }, button_two);
  ui_vaos.push_back(ui_vao);
  vertexCountsUI.push_back(button_two.positions.size());

  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glEnable(GL_DEPTH_TEST);


  // Benchmarking
  GLuint queries[6];
  glGenQueries(6, queries);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Let GLFW process events
    glfwPollEvents();

    // Calculate time
    std::chrono::steady_clock::time_point currentTime =
        std::chrono::steady_clock::now();
    std::chrono::duration<float> deltaTime =
        std::chrono::duration_cast<std::chrono::duration<float>>(currentTime -
                                                                 prevTime);
    prevTime = currentTime;
    float deltaTimeInSeconds = deltaTime.count();

    auto const now = Clock::now();
    float dt = std::chrono::duration_cast<Secondsf>(now - last).count();
    last = now;

    // Screen 0
    // Full render time start query
    glQueryCounter(queries[0], GL_TIMESTAMP);

    // Check if window was resized.
    float fbwidth, fbheight;
    {
      int nwidth, nheight;
      glfwGetFramebufferSize(window, &nwidth, &nheight);

      fbwidth = float(nwidth);
      fbheight = float(nheight);

      if (0 == nwidth || 0 == nheight) {
        // Window minimized? Pause until it is unminimized.
        // This is a bit of a hack.
        do {
          glfwWaitEvents();
          glfwGetFramebufferSize(window, &nwidth, &nheight);
        } while (0 == nwidth || 0 == nheight);
      }

      if (state.splitScreenActive)
        glViewport(0, 0, fbwidth / 2, fbheight);
      else
        glViewport(0, 0, fbwidth, fbheight);
    }

    Mat44f model2world = make_rotation_y(0);
    Mat44f world2camera = make_translation({0.f, 0.f, 0.f});
    Mat44f projection;

    if (!state.splitScreenActive) {
      projection = make_perspective_projection(
          60.f * kPi_ / 180.f, // Yes, a proper π would be useful. ( C++20:
                                    // mathematical constants)
          fbwidth / float(fbheight), 0.1f, 100.0f);
    }
    else {
      projection = make_perspective_projection(
        60.f * kPi_ / 180.f, // Yes, a proper π would be useful. ( C++20:
                                  // mathematical constants)
        (fbwidth/2) / float(fbheight), 0.1f, 100.0f);
    }

    Mat44f Rx, Ry, T;

    // Get Rx, Ry and T based on which camera is active

    if (state.mainCameraType == 0) // Main camera with movement
    {
      if (state.camControl.moveForward) {
        state.camControl.x -= state.camControl.speed * kMovementPerSecond_ * dt *
                              sin(state.camControl.phi) *
                              cos(state.camControl.theta);
        state.camControl.y += state.camControl.speed * kMovementPerSecond_ * dt *
                              sin(state.camControl.theta);
        state.camControl.z -= state.camControl.speed * kMovementPerSecond_ * dt *
                              cos(state.camControl.phi) *
                              cos(state.camControl.theta);
      } else if (state.camControl.moveBackward) {
        state.camControl.x += state.camControl.speed * kMovementPerSecond_ * dt *
                              sin(state.camControl.phi) *
                              cos(state.camControl.theta);
        state.camControl.y -= state.camControl.speed * kMovementPerSecond_ * dt *
                              sin(state.camControl.theta);
        state.camControl.z += state.camControl.speed * kMovementPerSecond_ * dt *
                              cos(state.camControl.phi) *
                              cos(state.camControl.theta);
      }

      if (state.camControl.moveLeft) {
        state.camControl.x += state.camControl.speed * kMovementPerSecond_ * dt *
                              sin(state.camControl.phi + kPi_ / 2.f);
        state.camControl.z += state.camControl.speed * kMovementPerSecond_ * dt *
                              cos(state.camControl.phi + kPi_ / 2.f);
      } else if (state.camControl.moveRight) {
        state.camControl.x -= state.camControl.speed * kMovementPerSecond_ * dt *
                              sin(state.camControl.phi + kPi_ / 2.f);
        state.camControl.z -= state.camControl.speed * kMovementPerSecond_ * dt *
                              cos(state.camControl.phi + kPi_ / 2.f);
      }

      Rx = make_rotation_x(state.camControl.theta);
      Ry = make_rotation_y(state.camControl.phi);

      T = make_translation(
          {state.camControl.x, state.camControl.y, -state.camControl.z});
    }
    else if (state.mainCameraType == 1) // Main camera following rocketship with x, y, z
    {
      state.mainTrackingCameraDynamic.x = spaceship.location.x + spaceship.offset.x;
      state.mainTrackingCameraDynamic.y = spaceship.location.y + spaceship.offset.y + 0.5;
      state.mainTrackingCameraDynamic.z = spaceship.location.z + spaceship.offset.z + 2;

      Vec3f direction = normalize(spaceship.location + spaceship.offset - Vec3f{ state.mainTrackingCameraDynamic.x, state.mainTrackingCameraDynamic.y, state.mainTrackingCameraDynamic.z });
      state.mainTrackingCameraDynamic.phi = atan2(direction.z, direction.x) + (kPi_/2); // angle is of by 90 degree so need to add pi/2
      state.mainTrackingCameraDynamic.theta = -atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));

      Rx = make_rotation_x(state.mainTrackingCameraDynamic.theta);
      Ry = make_rotation_y(state.mainTrackingCameraDynamic.phi);
      
      T = make_translation(
          {-state.mainTrackingCameraDynamic.x, -state.mainTrackingCameraDynamic.y, -state.mainTrackingCameraDynamic.z});
    }
    else if (state.mainCameraType == 2) // Main camera following rocketship with camera angle
    {
      state.mainTrackingCameraStatic.x = spaceship.location.x;
      state.mainTrackingCameraStatic.y = spaceship.location.y + 0.5;
      state.mainTrackingCameraStatic.z = spaceship.location.z + 2;

      Vec3f direction = normalize(spaceship.location + spaceship.offset - Vec3f{ state.mainTrackingCameraStatic.x, state.mainTrackingCameraStatic.y, state.mainTrackingCameraStatic.z });
      state.mainTrackingCameraStatic.phi = atan2(direction.z, direction.x) + (kPi_/2); // angle is of by 90 degree so need to add pi/2
      state.mainTrackingCameraStatic.theta = -atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));

      Rx = make_rotation_x(state.mainTrackingCameraStatic.theta);
      Ry = make_rotation_y(state.mainTrackingCameraStatic.phi);

      T = make_translation(
          {-state.mainTrackingCameraStatic.x, -state.mainTrackingCameraStatic.y, -state.mainTrackingCameraStatic.z});
    }

    world2camera = world2camera * (Rx * Ry * T);
    Mat44f projCameraWorld = projection * world2camera * model2world;

    Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
    
    // Draw scene
    OGL_CHECKPOINT_DEBUG();

    glClear(GL_COLOR_BUFFER_BIT);

    // Particle System
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (state.animation.animated) {
      state.animation.time += deltaTimeInSeconds;
    }
    if (state.animation.animated) {
      particleSystem.Update(deltaTimeInSeconds);
      particleSystem.Spawn(particle);
      particleSystem.Render(projCameraWorld);
    }

    glDisable(GL_BLEND);
    // Particle System end

    // Full render time start query
    glQueryCounter(queries[4], GL_TIMESTAMP);

    glUseProgram(prog.programId());

    glUniformMatrix4fv(2, 1, GL_TRUE, projCameraWorld.v);
    glUniformMatrix3fv(3, 1, GL_TRUE, normalMatrix.v);


    Vec3f lightDir = normalize(Vec3f{0.f, 1.f, -1.f});
    glUniform3fv(5, 1, &lightDir.x);
    glUniform3f(6, 0.9f, 0.9f, 0.6f);
    glUniform3f(7, 0.05f, 0.05f, 0.05f);

    static float const baseColor[] = {0.2f, 1.f, 1.f};
    glUniform3fv(0, 1, baseColor);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    for (unsigned int i = 0; i < vaos.size(); i++) {
      glBindVertexArray(vaos[i]);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textures[i]);
      glDrawArrays(GL_TRIANGLES, 0, vertexCounts[i]);
    }
    // Other render time end query
    glQueryCounter(queries[5], GL_TIMESTAMP);

    // Custom model render time start query
    glQueryCounter(queries[2], GL_TIMESTAMP);

    spaceship.render(projCameraWorld);

    // Custom model render time end query
    glQueryCounter(queries[3], GL_TIMESTAMP);

    spaceship.update(state.animation.time);
    particle.Position = spaceship.location + spaceship.offset;
    glUseProgram(prog.programId());

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glUseProgram(ui.programId());
    glDisable(GL_DEPTH_TEST);

    glUniform3fv(0, 1, baseColor);

    for (unsigned int i = 0; i < ui_vaos.size(); i++) {
      glBindVertexArray(ui_vaos[i]);
      glDrawArrays(GL_TRIANGLES, 0, vertexCountsUI[i]);
    }

    glBindVertexArray( 0 );
    glUseProgram( 0 );

    glEnable(GL_DEPTH_TEST);

    OGL_CHECKPOINT_DEBUG();
    
    // Screen 2
    if (state.splitScreenActive) {
      glViewport(fbwidth / 2, 0, fbwidth / 2, fbheight);

      // Update camera state
      // Assuming state.camControl.theta and state.camControl.phi are the camera
      // direction angles

      model2world = make_rotation_y(0);
      world2camera = make_translation({0.f, 0.f, 0.f});

      projection = make_perspective_projection(
          60.f * kPi_ / 180.f, // Yes, a proper π would be useful. ( C++20:
                                    // mathematical constants)
          (fbwidth/2) / float(fbheight), 0.1f, 100.0f);

      // Get Rx, Ry and T based on which camera is active

      if (state.splitCameraType == 0) { // Split camera with movement
        if (state.splitCam.moveForward) {
          state.splitCam.x -= state.splitCam.speed * kMovementPerSecond_ * dt *
                                sin(state.splitCam.phi) *
                                cos(state.splitCam.theta);
          state.splitCam.y += state.splitCam.speed * kMovementPerSecond_ * dt *
                                sin(state.splitCam.theta);
          state.splitCam.z -= state.splitCam.speed * kMovementPerSecond_ * dt *
                                cos(state.splitCam.phi) *
                                cos(state.splitCam.theta);
        } else if (state.splitCam.moveBackward) {
          state.splitCam.x += state.splitCam.speed * kMovementPerSecond_ * dt *
                                sin(state.splitCam.phi) *
                                cos(state.splitCam.theta);
          state.splitCam.y -= state.splitCam.speed * kMovementPerSecond_ * dt *
                                sin(state.splitCam.theta);
          state.splitCam.z += state.splitCam.speed * kMovementPerSecond_ * dt *
                                cos(state.splitCam.phi) *
                                cos(state.splitCam.theta);
        }

        if (state.splitCam.moveLeft) {
          state.splitCam.x += state.splitCam.speed * kMovementPerSecond_ * dt *
                                sin(state.splitCam.phi + kPi_ / 2.f);
          state.splitCam.z += state.splitCam.speed * kMovementPerSecond_ * dt *
                                cos(state.splitCam.phi + kPi_ / 2.f);
        } else if (state.splitCam.moveRight) {
          state.splitCam.x -= state.splitCam.speed * kMovementPerSecond_ * dt *
                                sin(state.splitCam.phi + kPi_ / 2.f);
          state.splitCam.z -= state.splitCam.speed * kMovementPerSecond_ * dt *
                                cos(state.splitCam.phi + kPi_ / 2.f);
        }
        Rx = make_rotation_x(state.splitCam.theta);
        Ry = make_rotation_y(state.splitCam.phi);

        T = make_translation(
          {state.splitCam.x, state.splitCam.y, -state.splitCam.z});
      }
      else if (state.splitCameraType == 1) // Split camera following rocketship with x, y, z
      {
        state.splitTrackingCameraDynamic.x = spaceship.location.x + spaceship.offset.x;
        state.splitTrackingCameraDynamic.y = spaceship.location.y + spaceship.offset.y + 0.5;
        state.splitTrackingCameraDynamic.z = spaceship.location.z + spaceship.offset.z + 2;

        Vec3f direction = normalize(spaceship.location + spaceship.offset - Vec3f{ state.splitTrackingCameraDynamic.x, state.splitTrackingCameraDynamic.y, state.splitTrackingCameraDynamic.z });
        state.splitTrackingCameraDynamic.phi = atan2(direction.z, direction.x) + (kPi_/2); // angle is of by 90 degree so need to add pi/2
        state.splitTrackingCameraDynamic.theta = -atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));

        Rx = make_rotation_x(state.splitTrackingCameraDynamic.theta);
        Ry = make_rotation_y(state.splitTrackingCameraDynamic.phi);
        
        T = make_translation(
            {-state.splitTrackingCameraDynamic.x, -state.splitTrackingCameraDynamic.y, -state.splitTrackingCameraDynamic.z});
      }
      else if (state.splitCameraType == 2) // Split camera following rocketship with camera angle
      {
        state.splitTrackingCameraStatic.x = spaceship.location.x;
        state.splitTrackingCameraStatic.y = spaceship.location.y + 0.5;
        state.splitTrackingCameraStatic.z = spaceship.location.z + 2;

        Vec3f direction = normalize(spaceship.location + spaceship.offset - Vec3f{ state.splitTrackingCameraStatic.x, state.splitTrackingCameraStatic.y, state.splitTrackingCameraStatic.z });
        state.splitTrackingCameraStatic.phi = atan2(direction.z, direction.x) + (kPi_/2); // angle is of by 90 degree so need to add pi/2
        state.splitTrackingCameraStatic.theta = -atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));

        Rx = make_rotation_x(state.splitTrackingCameraStatic.theta);
        Ry = make_rotation_y(state.splitTrackingCameraStatic.phi);

        T = make_translation(
            {-state.splitTrackingCameraStatic.x, -state.splitTrackingCameraStatic.y, -state.splitTrackingCameraStatic.z});
      }

      world2camera = world2camera * (Rx * Ry * T);
      projCameraWorld = projection * world2camera * model2world;

      normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
      lightDir = normalize(Vec3f{0.f, 1.f, -1.f});
      // Draw scene
      OGL_CHECKPOINT_DEBUG();

      // Particle System
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      if (state.animation.animated) {
        state.animation.time += deltaTimeInSeconds;
      }
      if (state.animation.animated) {
        particleSystem.Update(deltaTimeInSeconds);
        particleSystem.Spawn(particle);
        particleSystem.Render(projCameraWorld);
      }

      glDisable(GL_BLEND);
      // Particle System end

      glUseProgram(prog.programId());

      glUniformMatrix4fv(2, 1, GL_TRUE, projCameraWorld.v);
      glUniformMatrix3fv(3, 1, GL_TRUE, normalMatrix.v);

      glUniform3fv(5, 1, &lightDir.x);
      glUniform3f(6, 0.9f, 0.9f, 0.6f);
      glUniform3f(7, 0.05f, 0.05f, 0.05f);

      glUniform3fv(0, 1, baseColor);

      glBindTexture(GL_TEXTURE_2D, 0);
      glBindVertexArray(0);

      for (unsigned int i = 0; i < vaos.size(); i++) {
        glBindVertexArray(vaos[i]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDrawArrays(GL_TRIANGLES, 0, vertexCounts[i]);
      }

      spaceship.render(projCameraWorld);
      spaceship.update(state.animation.time);
      particle.Position = spaceship.location + spaceship.offset;

      glBindTexture(GL_TEXTURE_2D, 0);
      glBindVertexArray(0);
      glUseProgram(0);

     

      OGL_CHECKPOINT_DEBUG();
    }

    // Full render time end query
    glQueryCounter(queries[1], GL_TIMESTAMP);

    // End Screens

    // Get timestamp queries for benchmarking
    GLuint64 fullRenderStartTime, fullRenderEndTime, customRenderStartTime, customRenderEndTime, otherRenderStartTime, otherRenderEndTime;
    glGetQueryObjectui64v(queries[0], GL_QUERY_RESULT, &fullRenderStartTime);
    glGetQueryObjectui64v(queries[1], GL_QUERY_RESULT, &fullRenderEndTime);
    glGetQueryObjectui64v(queries[2], GL_QUERY_RESULT, &customRenderStartTime);
    glGetQueryObjectui64v(queries[3], GL_QUERY_RESULT, &customRenderEndTime);
    glGetQueryObjectui64v(queries[4], GL_QUERY_RESULT, &otherRenderStartTime);
    glGetQueryObjectui64v(queries[5], GL_QUERY_RESULT, &otherRenderEndTime);

    // Calculate time delta
    double fRenderTime = (fullRenderEndTime - fullRenderStartTime); // time in nanoseconds
    double cRenderTime = (customRenderEndTime - customRenderStartTime);
    double oRenderTime = (otherRenderEndTime - otherRenderStartTime);

    // Push deltas to respective vectors
    fullRenderTime.push_back(fRenderTime);
    customRenderTime.push_back(cRenderTime);
    otherRenderTime.push_back(oRenderTime);
    
    // Display results
    glfwSwapBuffers(window);
  }

  // Cleanup.
  // TODO: additional cleanup

  return 0;
} catch (std::exception const &eErr) {
  std::fprintf(stderr, "Top-level Exception (%s):\n", typeid(eErr).name());
  std::fprintf(stderr, "%s\n", eErr.what());
  std::fprintf(stderr, "Bye.\n");
  return 1;
}

namespace {
void glfw_callback_error_(int aErrNum, char const *aErrDesc) {
  std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
}

void glfw_callback_key_(GLFWwindow *aWindow, int aKey, int, int aAction, int mods) {
  if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction) {
    glfwSetWindowShouldClose(aWindow, GLFW_TRUE);

    // Write render times from vectors to respective csv files
    std::ofstream myfile;
    int vsize;

    myfile.open("fullTime.csv");
    vsize = fullRenderTime.size();
    for (int n=0; n<vsize; n++)
    {
        myfile << fullRenderTime[n] << std::endl;
    }
    myfile.close();

    myfile.open("customModelTime.csv");
    vsize = customRenderTime.size();
    for (int n=0; n<vsize; n++)
    {
        myfile << customRenderTime[n] << std::endl;
    }
    myfile.close();

    myfile.open("oTime.csv");
    vsize = otherRenderTime.size();
    for (int n=0; n<vsize; n++)
    {
        myfile << otherRenderTime[n] << std::endl;
    }
    myfile.close();

    return;
  }

  if (auto *state = static_cast<State_ *>(glfwGetWindowUserPointer(aWindow))) {
    // R-key resets animation.
    if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction) {
      state->animation.animated = false;
      state->animation.time = 0;
    }
    // F-key starts animation.
    if (GLFW_KEY_F == aKey && GLFW_PRESS == aAction) {
      state->animation.animated = true;
    }
    // Space toggles camera
    if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction) {
      state->camControl.cameraActive = !state->camControl.cameraActive;
      state->splitCam.cameraActive = !state->splitCam.cameraActive;

      if (state->camControl.cameraActive || state->splitCam.cameraActive)
        glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      else
        glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // V Splits the screen
    if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction) {
      if (state->splitScreenActive == 0)
        state->splitScreenActive = 1;
      else
        state->splitScreenActive = 0;
    }
    // C and shift-C changes cameras
    if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction && mods != GLFW_MOD_SHIFT) {
      state->activeScreen = 0;

      // Stop other cam from moving
      state->splitCam.moveForward = false;
      state->splitCam.moveBackward = false;
      state->splitCam.moveLeft = false;
      state->splitCam.moveRight = false;

      // Cycle camera type for main camera
      if (state->mainCameraType == 0)
        state->mainCameraType = 1;
      else if (state->mainCameraType == 1)
        state->mainCameraType = 2;
      else if (state->mainCameraType == 2)
        state->mainCameraType = 0;
    }
    if (state->splitScreenActive && GLFW_KEY_C == aKey && GLFW_PRESS == aAction && mods == GLFW_MOD_SHIFT) {
      state->activeScreen = 1;

      // Stop other cam from moving
      state->camControl.moveForward = false;
      state->camControl.moveBackward = false;
      state->camControl.moveLeft = false;
      state->camControl.moveRight = false;

      // Cycle camera type for split camera
      if (state->splitCameraType == 0)
        state->splitCameraType = 1;
      else if (state->splitCameraType == 1)
        state->splitCameraType = 2;
      else if (state->splitCameraType == 2)
        state->splitCameraType = 0;
    }

    // Camera controls if camera is active
    if (state->activeScreen == 0 && state->camControl.cameraActive) {
      if (GLFW_KEY_W == aKey) {
        if (GLFW_PRESS == aAction)
          state->camControl.moveForward = true;
        else if (GLFW_RELEASE == aAction)
          state->camControl.moveForward = false;
      } else if (GLFW_KEY_S == aKey) {
        if (GLFW_PRESS == aAction)
          state->camControl.moveBackward = true;
        else if (GLFW_RELEASE == aAction)
          state->camControl.moveBackward = false;
      }
      if (GLFW_KEY_A == aKey) {
        if (GLFW_PRESS == aAction)
          state->camControl.moveLeft = true;
        else if (GLFW_RELEASE == aAction)
          state->camControl.moveLeft = false;
      } else if (GLFW_KEY_D == aKey) {
        if (GLFW_PRESS == aAction)
          state->camControl.moveRight = true;
        else if (GLFW_RELEASE == aAction)
          state->camControl.moveRight = false;
      }
      if (GLFW_KEY_LEFT_SHIFT == aKey) {
        if (GLFW_PRESS == aAction)
          state->camControl.speed = 1.5f;
        else if (GLFW_RELEASE == aAction)
          state->camControl.speed = 1.f;
      } else if (GLFW_KEY_LEFT_CONTROL == aKey) {
        if (GLFW_PRESS == aAction)
          state->camControl.speed = 0.5f;
        else if (GLFW_RELEASE == aAction)
          state->camControl.speed = 1.f;
      }
    }
    else if (state->activeScreen == 1 && state->splitCam.cameraActive) {
      if (GLFW_KEY_W == aKey) {
        if (GLFW_PRESS == aAction)
          state->splitCam.moveForward = true;
        else if (GLFW_RELEASE == aAction)
          state->splitCam.moveForward = false;
      } else if (GLFW_KEY_S == aKey) {
        if (GLFW_PRESS == aAction)
          state->splitCam.moveBackward = true;
        else if (GLFW_RELEASE == aAction)
          state->splitCam.moveBackward = false;
      }
      if (GLFW_KEY_A == aKey) {
        if (GLFW_PRESS == aAction)
          state->splitCam.moveLeft = true;
        else if (GLFW_RELEASE == aAction)
          state->splitCam.moveLeft = false;
      } else if (GLFW_KEY_D == aKey) {
        if (GLFW_PRESS == aAction)
          state->splitCam.moveRight = true;
        else if (GLFW_RELEASE == aAction)
          state->splitCam.moveRight = false;
      }
      if (GLFW_KEY_LEFT_SHIFT == aKey) {
        if (GLFW_PRESS == aAction)
          state->splitCam.speed = 1.5f;
        else if (GLFW_RELEASE == aAction)
          state->splitCam.speed = 1.f;
      }
    }
  }
}

void glfw_callback_motion_(GLFWwindow *aWindow, double aX, double aY) {
  if (auto *state = static_cast<State_ *>(glfwGetWindowUserPointer(aWindow))) {
    if (state->activeScreen == 0) {
      if (state->camControl.cameraActive) {
        auto const dx = float(aX - state->camControl.lastX);
        auto const dy = float(aY - state->camControl.lastY);

        state->camControl.phi += dx * kMouseSensitivity_;

        state->camControl.theta += dy * kMouseSensitivity_;
        if (state->camControl.theta > kPi_ / 2.f)
          state->camControl.theta = kPi_ / 2.f;
        else if (state->camControl.theta < -kPi_ / 2.f)
          state->camControl.theta = -kPi_ / 2.f;
      }
      state->camControl.lastX = float(aX);
      state->camControl.lastY = float(aY);
    }
    else if (state->activeScreen == 1) {
      if (state->splitCam.cameraActive) {
        auto const dx = float(aX - state->splitCam.lastX);
        auto const dy = float(aY - state->splitCam.lastY);

        state->splitCam.phi += dx * kMouseSensitivity_;

        state->splitCam.theta += dy * kMouseSensitivity_;
        if (state->splitCam.theta > kPi_ / 2.f)
          state->splitCam.theta = kPi_ / 2.f;
        else if (state->splitCam.theta < -kPi_ / 2.f)
          state->splitCam.theta = -kPi_ / 2.f;
      }
      state->splitCam.lastX = float(aX);
      state->splitCam.lastY = float(aY);
    }
  }
}
  void mouse_click_callback_(GLFWwindow* window, int button, int action, int mods) {
    if (auto *state = static_cast<State_ *>(glfwGetWindowUserPointer(window))) {
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        double xpos = 0, ypos = 0;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float rect_left = -0.4f;
        float rect_right = -0.1f;
        float rect_top = -0.6f;
        float rect_bottom = -0.8f;

        int rectangle_x_one = static_cast<int>((rect_left + 1.0) * 0.5 * width);
        int rectangle_x_two = static_cast<int>((rect_right + 1.0) * 0.5 * width);
        int rectangle_y_one = static_cast<int>((1.0 - rect_top) * 0.5 * height);
        int rectangle_y_two = static_cast<int>((1.0 - rect_bottom) * 0.5 * height);

        float rect_left_two = 0.1f;
        float rect_right_two = 0.4f;
        float rect_top_two = -0.6f;
        float rect_bottom_two = -0.8f;

        int rectangle_two_x_one = static_cast<int>((rect_left_two + 1.0) * 0.5 * width);
        int rectangle_two_x_two = static_cast<int>((rect_right_two + 1.0) * 0.5 * width);
        int rectangle_two_y_one = static_cast<int>((1.0 - rect_top_two) * 0.5 * height);
        int rectangle_two_y_two = static_cast<int>((1.0 - rect_bottom_two) * 0.5 * height);

        if (rectangle_x_one <= xpos && rectangle_x_two >= xpos && 
            rectangle_y_one <= ypos && rectangle_y_two >= ypos) {
          // In left rectangle, launch spaceship
          state->animation.animated = true;
        }
        else if (rectangle_two_x_one <= xpos && rectangle_two_x_two >= xpos && 
                rectangle_two_y_one <= ypos && rectangle_two_y_two >= ypos) {
          // In right rectangle, reset spaceship
          state->animation.animated = false;
          state->animation.time = 0;
        }
      }
    }
  }
}
 // namespace

namespace {
GLFWCleanupHelper::~GLFWCleanupHelper() { glfwTerminate(); }

GLFWWindowDeleter::~GLFWWindowDeleter() {
  if (window)
    glfwDestroyWindow(window);
}
} // namespace
