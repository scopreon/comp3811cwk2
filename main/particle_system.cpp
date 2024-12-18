#include "particle_system.hpp"

// Generate random float between 0 and 1
float RandomFloat01() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen);
}

// Linear interpolation function betwen two floats
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

// Constructor sets ParticlePool vector to size 1000
ParticleSystem::ParticleSystem()
{
	particlePool.resize(1000);
}

// Update particles function
void ParticleSystem::Update(float ts)
{
	for (auto& particle : particlePool)
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

// Render particles
void ParticleSystem::Render(Mat44f projCameraWorld)
{
    ShaderProgram prog({{GL_VERTEX_SHADER, "assets/particle.vert"},
                        {GL_FRAGMENT_SHADER, "assets/particle.frag"}});


    if (!cubeVA)
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

        glGenVertexArrays(1, &cubeVA);
        glBindVertexArray(cubeVA);

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

	for (auto& particle : particlePool)
	{
		if (!particle.Active)
			continue;

		// Fade away particles
		float life = particle.LifeRemaining / particle.LifeTime;
		float size = lerp(particle.SizeEnd, particle.SizeBegin, life);
        Vec4f color = lerp(particle.ColorEnd, particle.ColorBegin, life);
		
		// Render
        Mat44f transform = make_translation(Vec3f{particle.Position.x, particle.Position.y, particle.Position.z})
            * make_rotation_x(particle.Rotation)
            * make_rotation_y(particle.Rotation)
            * make_rotation_z(particle.Rotation)
            * make_scaling(size, size, size);

		glUniformMatrix4fv(1, 1, GL_TRUE, transform.v);
		glUniform4f(2,color.x, color.y, color.z, color.w);
		glBindVertexArray(cubeVA);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	}
}

// Spawn particles
void ParticleSystem::Spawn(const ParticleInit& particleInit)
{   
    // Get current index of particle pool
	Particle& particle = particlePool[poolIndex];

    // Set particle to active
	particle.Active = true;

    // Set position
	particle.Position = particleInit.Position;
    particle.Position.x += particleInit.PositionVariation.x * (RandomFloat01() - 0.5f);
    particle.Position.y += particleInit.PositionVariation.y * (RandomFloat01() - 0.5f);
    particle.Position.z += particleInit.PositionVariation.z * (RandomFloat01() - 0.5f);

    // Set rotation
	particle.Rotation = RandomFloat01() * 2.0f * 3.14;

	// Velocity
	particle.Velocity = particleInit.Velocity;
    particle.Velocity.x += particleInit.VelocityVariation.x * (RandomFloat01() - 0.5f);
    particle.Velocity.y += particleInit.VelocityVariation.y * (RandomFloat01() - 0.5f);
    particle.Velocity.z += particleInit.VelocityVariation.z * (RandomFloat01() - 0.5f);

	// Set color
	particle.ColorBegin = particleInit.ColorBegin;
	particle.ColorEnd = particleInit.ColorEnd;
    
    // Set life time
	particle.LifeTime = particleInit.LifeTime;
	particle.LifeRemaining = particleInit.LifeTime;

    // Set size
	particle.SizeBegin = particleInit.SizeBegin - particleInit.SizeVariation + (particleInit.SizeVariation * RandomFloat01());
	particle.SizeEnd = particleInit.SizeEnd;

    // Update pool index
    unsigned int newIndex = --poolIndex % particlePool.size();
	poolIndex = newIndex;
}
