#pragma once

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include <glad.h>
#include "../support/program.hpp"

#include <vector>
#include <memory>
#include <random>

struct ParticleInit
{
	Vec3f Position, PositionVariation;
	Vec3f Velocity, VelocityVariation;
	Vec4f ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
};

class ParticleSystem
{
public:
    ParticleSystem();

    void Update(float ts);
    void Render(Mat44f projCameraWorld);

    void Spawn(const ParticleInit& particleInit);
private:
	struct Particle
	{
		Vec3f Position;
		Vec3f Velocity;
		Vec4f ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};
	std::vector<Particle> particlePool;
	uint32_t poolIndex = 999;

	GLuint cubeVA = 0;
};