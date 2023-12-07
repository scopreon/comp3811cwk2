#pragma once

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include <glad.h>
#include "../support/program.hpp"

#include <vector>
#include <memory>
#include <random>

struct ParticleProps
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

    void OnUpdate(float ts);
    void OnRender(Mat44f projCameraWorld);

    void Emit(const ParticleProps& particleProps);
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
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = 999;

	GLuint m_CubeVA = 0;
	GLint m_ParticleShaderViewProj, m_ParticleShaderTransform, m_ParticleShaderColor;
};