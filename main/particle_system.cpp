#include "particle_system.hpp"

#include <random>

float RandomFloat01() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}

// Linear interpolation function between two Vec4f
Vec4f lerp(const Vec4f& a, const Vec4f& b, float t) {
    Vec4f result;
    result.x = a.x * (1.0f - t) + b.x * t;
    result.y = a.y * (1.0f - t) + b.y * t;
    result.z = a.z * (1.0f - t) + b.z * t;
    result.w = a.w * (1.0f - t) + b.w * t;
    return result;
}

ParticleSystem::ParticleSystem()
{
	m_ParticlePool.resize(1000);
}

void ParticleSystem::OnUpdate(float ts)
{
	for (auto& particle : m_ParticlePool)
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= ts;
		particle.Position += particle.Velocity * (float)ts;
		particle.Rotation += 0.01f * ts;
	}
}

void ParticleSystem::OnRender(Mat44f projCameraWorld)
{
    ShaderProgram prog({{GL_VERTEX_SHADER, "assets/shader.glsl.vert"},
                      {GL_FRAGMENT_SHADER, "assets/shader.glsl.frag"}});


if (!m_CubeVA)
{
float vertices[] = {
    // Front face
    -0.05f, -0.05f, 0.05f,
     0.05f, -0.05f, 0.05f,
     0.05f,  0.05f, 0.05f,
    -0.05f,  0.05f, 0.05f,

    // Back face
    -0.05f, -0.05f, -0.05f,
     0.05f, -0.05f, -0.05f,
     0.05f,  0.05f, -0.05f,
    -0.05f,  0.05f, -0.05f,

    // Left face
    -0.05f, -0.05f, -0.05f,
    -0.05f, -0.05f,  0.05f,
    -0.05f,  0.05f,  0.05f,
    -0.05f,  0.05f, -0.05f,

    // Right face
     0.05f, -0.05f, -0.05f,
     0.05f, -0.05f,  0.05f,
     0.05f,  0.05f,  0.05f,
     0.05f,  0.05f, -0.05f,

    // Top face
    -0.05f,  0.05f,  0.05f,
     0.05f,  0.05f,  0.05f,
     0.05f,  0.05f, -0.05f,
    -0.05f,  0.05f, -0.05f,

    // Bottom face
    -0.05f, -0.05f,  0.05f,
     0.05f, -0.05f,  0.05f,
     0.05f, -0.05f, -0.05f,
    -0.05f, -0.05f, -0.05f
};

    glGenVertexArrays(1, &m_CubeVA);
    glBindVertexArray(m_CubeVA);

    GLuint cubeVB, cubeIB;
    glGenBuffers(1, &cubeVB);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    uint32_t indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    glGenBuffers(1, &cubeIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

	glUseProgram(prog.programId());
	glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);

	for (auto& particle : m_ParticlePool)
	{
		if (!particle.Active)
			continue;

		// Fade away particles
		float life = particle.LifeRemaining / particle.LifeTime;
		float size = lerp(particle.SizeEnd, particle.SizeBegin, life);
        Vec4f color = lerp(particle.ColorEnd, particle.ColorBegin, life);
        

        printf("size: %f\n", size);
		
		// Render
        Mat44f transform = make_translation(Vec3f{particle.Position.x, particle.Position.y, particle.Position.z})
            * make_rotation_x(particle.Rotation)
            * make_rotation_y(particle.Rotation)
            * make_rotation_z(particle.Rotation)
            * make_scaling(size, size, size);

        // Mat44f transform = make_scaling(size, size, size);


		glUniformMatrix4fv(1, 1, GL_TRUE, transform.v);
		glUniform4f(2,color.x, color.y, color.z, color.w);
		glBindVertexArray(m_CubeVA);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	}
    printf("yooo\n");
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
	Particle& particle = m_ParticlePool[m_PoolIndex];
	particle.Active = true;
	particle.Position = particleProps.Position;
    // CHANGE PI
	// particle.Rotation = Random::Float() * 2.0f * 3.14;

	// Velocity
	particle.Velocity = particleProps.Velocity;
    particle.Velocity.x += particleProps.VelocityVariation.x * (RandomFloat01() - 0.5f);
    particle.Velocity.y += particleProps.VelocityVariation.y * (RandomFloat01() - 0.5f);


	// Color
	particle.ColorBegin = particleProps.ColorBegin;
	particle.ColorEnd = particleProps.ColorEnd;

	particle.LifeTime = particleProps.LifeTime;
	particle.LifeRemaining = particleProps.LifeTime;
	particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (RandomFloat01() - 0.5f);
	particle.SizeEnd = particleProps.SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}
