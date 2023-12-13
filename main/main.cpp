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


  CamCtrl_ secondCam;

  CamCtrl_ trackingCameraDynamic;
  CamCtrl_ trackingCameraStatic;

  bool splitScreenActive = 0;
  bool activeCamera = 0; // false/0 for screen 0; true/1 for screen 1


  unsigned int cameraType = 0; // 0 = normal; 1 = tracking camera dynamic; 2 = tracking camera static

  struct Animation_ {
    bool animated;
    float time;
  } animation;
};

void glfw_callback_error_(int, char const *);

void glfw_callback_key_(GLFWwindow *, int, int, int, int);
void glfw_callback_motion_(GLFWwindow *, double, double);

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

  // TODO: global GL setup goes here

  glEnable(GL_FRAMEBUFFER_SRGB);
  //   glEnable(GL_CULL_FACE);

  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

  OGL_CHECKPOINT_ALWAYS();

  // Get actual framebuffer size.
  // This can be different from the window size, as standard window
  // decorations (title bar, borders, ...) may be included in the window size
  // but not be part of the drawable surface area.
  int iwidth, iheight;
  glfwGetFramebufferSize(window, &iwidth, &iheight);

  glViewport(0, 0, iwidth, iheight);

  ShaderProgram prog({{GL_VERTEX_SHADER, "assets/default.vert"},
                      {GL_FRAGMENT_SHADER, "assets/default.frag"}});

  state.prog = &prog;
  //   state.camControl.radius = 10.f;

  auto last = Clock::now();

  float angle = 0.f;

  std::vector<GLuint> vaos;
  std::vector<std::size_t> vertexCounts;
  std::vector<GLuint> textures;
  std::size_t vertexCount = 0;

  GLuint tex = load_texture_2d("assets/L4343A-4k.jpeg");

  auto map = load_wavefront_obj("assets/parlahti.obj");
  GLuint vao = create_vao(map);
  vaos.push_back(vao);
  vertexCounts.push_back(map.positions.size());
  textures.push_back(tex);

  auto launchhpad = load_wavefront_obj("assets/landingpad.obj");
  for (auto &p : launchhpad.positions) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = make_translation(Vec3f{-10.f, -0.97f, 15.f}) * p4;
    t /= t.w;

    Vec3f pTransformed{t.x, t.y, t.z};
    p = pTransformed;
  }

  vao = create_vao(launchhpad);
  vaos.push_back(vao);
  vertexCounts.push_back(launchhpad.positions.size());
  textures.push_back(0);

  launchhpad = load_wavefront_obj("assets/landingpad.obj");
  for (auto &p : launchhpad.positions) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = make_translation(Vec3f{-50.f, -0.97f, 20.f}) * p4;
    t /= t.w;

    Vec3f pTransformed{t.x, t.y, t.z};
    p = pTransformed;
  }

  vao = create_vao(launchhpad);
  vaos.push_back(vao);
  vertexCounts.push_back(launchhpad.positions.size());
  textures.push_back(0);
  // return 0;

  //   auto spaceship =
  //       make_spaceship(10, kIdentity44f * make_translation({-10.f,
  //       -0.9f, 15.f}) *
  //                              make_scaling(0.1f, 0.1f, 0.1f));
  // return 0;
  //   for (const auto &p : shape.normals) {
  //     printf("%f, %f, %f\n", p.x, p.y, p.z);
  //   }

  Spaceship spaceship(10, kIdentity44f *
                              make_translation({-10.f, -0.9f, 15.f}) *
                              make_scaling(0.1f, 0.1f, 0.1f));
  //   vao = create_vao(spaceship);
  //   vaos.push_back(vao);
  //   vertexCounts.push_back(spaceship.positions.size());
  //   textures.push_back(0);

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
  std::chrono::steady_clock::time_point initialTime =
      std::chrono::steady_clock::now();

  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glEnable(GL_DEPTH_TEST);

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

    // angle += dt * kPi_ * 0.3f;
    // if (angle >= 2.f * kPi_)
    //   angle -= 2.f * kPi_;

    // Update camera state
    // Assuming state.camControl.theta and state.camControl.phi are the camera
    // direction angles


    // If statement depending on camera

    Mat44f model2world = make_rotation_y(0);
    Mat44f world2camera = make_translation({0.f, 0.f, 0.f});

    Mat44f projection = make_perspective_projection(
        60.f * kPi_ / 180.f, // Yes, a proper π would be useful. ( C++20:
                                   // mathematical constants)
        fbwidth / float(fbheight), 0.1f, 100.0f);

    Mat44f Rx, Ry, T;

    if (state.cameraType == 0)
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
    else if (state.cameraType == 1)
    {
      state.trackingCameraDynamic.x = spaceship.location.x + spaceship.offset.x;
      state.trackingCameraDynamic.y = spaceship.location.y + spaceship.offset.y + 0.25;
      state.trackingCameraDynamic.z = spaceship.location.z + spaceship.offset.z + 1.5;

      Rx = make_rotation_x(state.trackingCameraDynamic.theta);
      Ry = make_rotation_y(state.trackingCameraDynamic.phi);
      
      T = make_translation(
          {-state.trackingCameraDynamic.x, -state.trackingCameraDynamic.y, -state.trackingCameraDynamic.z});
    }
    else if (state.cameraType == 2)
    {

      state.trackingCameraStatic.x = spaceship.location.x;
      state.trackingCameraStatic.y = spaceship.location.y + 0.25;
      state.trackingCameraStatic.z = spaceship.location.z + 1.5;


      Vec3f direction = normalize(spaceship.location + spaceship.offset - Vec3f{ state.trackingCameraStatic.x, state.trackingCameraStatic.y, state.trackingCameraStatic.z });
      state.trackingCameraStatic.phi = atan2(direction.z, direction.x) + (kPi_/2); // angle is of by 90 degree so need to add pi/2
      state.trackingCameraStatic.theta = -atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));



      Rx = make_rotation_x(state.trackingCameraStatic.theta);
      Ry = make_rotation_y(state.trackingCameraStatic.phi);

      T = make_translation(
          {-state.trackingCameraStatic.x, -state.trackingCameraStatic.y, -state.trackingCameraStatic.z});
    }

    // End if



    world2camera = world2camera * (Rx * Ry * T);
    Mat44f projCameraWorld = projection * world2camera * model2world;

    Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2world)));
    Vec3f lightDir = normalize(Vec3f{0.f, 1.f, -1.f});
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

    glUseProgram(prog.programId());

    glUniformMatrix4fv(2, 1, GL_TRUE, projCameraWorld.v);
    glUniformMatrix3fv(3, 1, GL_TRUE, normalMatrix.v);

    glUniform3fv(5, 1, &lightDir.x);
    glUniform3f(6, 0.9f, 0.9f, 0.6f);
    glUniform3f(7, 0.05f, 0.05f, 0.05f);

    static float const baseColor[] = {0.2f, 1.f, 1.f};
    glUniform3fv(0, 1, baseColor);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    for (int i = 0; i < vaos.size(); i++) {
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
    
    // Screen 2
    if (state.splitScreenActive) {
      glViewport(fbwidth / 2, 0, fbwidth / 2, fbheight);

      // Update camera state
      // Assuming state.camControl.theta and state.camControl.phi are the camera
      // direction angles

      if (state.secondCam.moveForward) {
        state.secondCam.x -= state.secondCam.speed * kMovementPerSecond_ * dt *
                              sin(state.secondCam.phi) *
                              cos(state.secondCam.theta);
        state.secondCam.y += state.secondCam.speed * kMovementPerSecond_ * dt *
                              sin(state.secondCam.theta);
        state.secondCam.z -= state.secondCam.speed * kMovementPerSecond_ * dt *
                              cos(state.secondCam.phi) *
                              cos(state.secondCam.theta);
      } else if (state.secondCam.moveBackward) {
        state.secondCam.x += state.secondCam.speed * kMovementPerSecond_ * dt *
                              sin(state.secondCam.phi) *
                              cos(state.secondCam.theta);
        state.secondCam.y -= state.secondCam.speed * kMovementPerSecond_ * dt *
                              sin(state.secondCam.theta);
        state.secondCam.z += state.secondCam.speed * kMovementPerSecond_ * dt *
                              cos(state.secondCam.phi) *
                              cos(state.secondCam.theta);
      }

      if (state.secondCam.moveLeft) {
        state.secondCam.x += state.secondCam.speed * kMovementPerSecond_ * dt *
                              sin(state.secondCam.phi + kPi_ / 2.f);
        state.secondCam.z += state.secondCam.speed * kMovementPerSecond_ * dt *
                              cos(state.secondCam.phi + kPi_ / 2.f);
      } else if (state.secondCam.moveRight) {
        state.secondCam.x -= state.secondCam.speed * kMovementPerSecond_ * dt *
                              sin(state.secondCam.phi + kPi_ / 2.f);
        state.secondCam.z -= state.secondCam.speed * kMovementPerSecond_ * dt *
                              cos(state.secondCam.phi + kPi_ / 2.f);
      }

      model2world = make_rotation_y(0);
      world2camera = make_translation({0.f, 0.f, 0.f});

      projection = make_perspective_projection(
          60.f * kPi_ / 180.f, // Yes, a proper π would be useful. ( C++20:
                                    // mathematical constants)
          fbwidth / float(fbheight), 0.1f, 100.0f);

      Rx = make_rotation_x(state.secondCam.theta);
      Ry = make_rotation_y(state.secondCam.phi);

      T = make_translation(
          {state.secondCam.x, state.secondCam.y, -state.secondCam.z});
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

      for (int i = 0; i < vaos.size(); i++) {
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
    // End Screens
    
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
    return;
  }

  if (auto *state = static_cast<State_ *>(glfwGetWindowUserPointer(aWindow))) {
    // R-key reloads shaders.
    if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction) {
      //   if (state->prog) {
      //     try {
      //       state->prog->reload();
      //       std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
      //     } catch (std::exception const &eErr) {
      //       std::fprintf(stderr, "Error when reloading shader:\n");
      //       std::fprintf(stderr, "%s\n", eErr.what());
      //       std::fprintf(stderr, "Keeping old shader.\n");
      //     }
      //   }
      state->animation.animated = false;
      state->animation.time = 0;
    }
    if (GLFW_KEY_F == aKey && GLFW_PRESS == aAction) {
      state->animation.animated = true;
      //   state->animation.time = 0;
    }
    // Space toggles camera
    if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction) {
      state->camControl.cameraActive = !state->camControl.cameraActive;
      state->secondCam.cameraActive = !state->secondCam.cameraActive;

      if (state->camControl.cameraActive || state->secondCam.cameraActive)
        glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      else
        glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // C cycles through cameras for screen 0
    if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction) {
      if (state->cameraType == 0)
        state->cameraType = 1;
      else if (state->cameraType == 1)
        state->cameraType = 2;
      else if (state->cameraType == 2)
        state->cameraType = 0;
    }
    // V Splits the screen
    if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction) {
      if (state->splitScreenActive == 0)
        state->splitScreenActive = 1;
      else
        state->splitScreenActive = 0;
    }
    // C and shift-C changes cameras
    if (state->splitScreenActive == 0 || (GLFW_KEY_C == aKey && GLFW_PRESS == aAction)) {
      state->activeCamera = 0;
    }
    if (state->splitScreenActive && GLFW_KEY_C == aKey && GLFW_PRESS == aAction && mods == GLFW_MOD_SHIFT) {
      state->activeCamera = 1;
    }

    // Camera controls if camera is active
    if (state->activeCamera == 0 && state->camControl.cameraActive) {
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
      }
    }
    else if (state->activeCamera == 1 && state->secondCam.cameraActive) {
      if (GLFW_KEY_W == aKey) {
        if (GLFW_PRESS == aAction)
          state->secondCam.moveForward = true;
        else if (GLFW_RELEASE == aAction)
          state->secondCam.moveForward = false;
      } else if (GLFW_KEY_S == aKey) {
        if (GLFW_PRESS == aAction)
          state->secondCam.moveBackward = true;
        else if (GLFW_RELEASE == aAction)
          state->secondCam.moveBackward = false;
      }
      if (GLFW_KEY_A == aKey) {
        if (GLFW_PRESS == aAction)
          state->secondCam.moveLeft = true;
        else if (GLFW_RELEASE == aAction)
          state->secondCam.moveLeft = false;
      } else if (GLFW_KEY_D == aKey) {
        if (GLFW_PRESS == aAction)
          state->secondCam.moveRight = true;
        else if (GLFW_RELEASE == aAction)
          state->secondCam.moveRight = false;
      }
      if (GLFW_KEY_LEFT_SHIFT == aKey) {
        if (GLFW_PRESS == aAction)
          state->secondCam.speed = 1.5f;
        else if (GLFW_RELEASE == aAction)
          state->secondCam.speed = 1.f;
      }
    }
  }
}

void glfw_callback_motion_(GLFWwindow *aWindow, double aX, double aY) {
  if (auto *state = static_cast<State_ *>(glfwGetWindowUserPointer(aWindow))) {
    if (state->activeCamera == 0) {
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
    else if (state->activeCamera == 1) {
      if (state->secondCam.cameraActive) {
        auto const dx = float(aX - state->secondCam.lastX);
        auto const dy = float(aY - state->secondCam.lastY);

        state->secondCam.phi += dx * kMouseSensitivity_;

        state->secondCam.theta += dy * kMouseSensitivity_;
        if (state->secondCam.theta > kPi_ / 2.f)
          state->secondCam.theta = kPi_ / 2.f;
        else if (state->secondCam.theta < -kPi_ / 2.f)
          state->secondCam.theta = -kPi_ / 2.f;
      }
      state->secondCam.lastX = float(aX);
      state->secondCam.lastY = float(aY);
    }
  }
}
} // namespace

namespace {
GLFWCleanupHelper::~GLFWCleanupHelper() { glfwTerminate(); }

GLFWWindowDeleter::~GLFWWindowDeleter() {
  if (window)
    glfwDestroyWindow(window);
}
} // namespace
