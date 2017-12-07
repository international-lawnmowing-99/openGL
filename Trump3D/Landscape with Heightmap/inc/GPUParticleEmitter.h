#pragma once
#include <glm\common.hpp>
//#include "ShaderLoader.h"

struct GPUParticle;

class GPUParticleEmitter
{
public:
	GPUParticleEmitter();
	virtual ~GPUParticleEmitter();

	void Initialise(unsigned int maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endsize, const glm::vec4& startColour, const glm::vec4& endColour);
	void Draw(float time, const glm::mat4& cameraTransform, const glm::mat4& projectionView);
protected:
	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	GPUParticle * m_particles;

	unsigned int m_maxParticles;
	glm::vec3 m_position;

	float m_lifespanMax, m_lifespanMin;
	float m_velocityMax, m_velocityMin;

	float m_startSize, m_endSize;

	glm::vec4 m_startColour, m_endColour;

	unsigned int m_activeBuffer;
	unsigned int m_vao[2];
	unsigned int m_vbo[2];

	unsigned int m_updateShader;
	unsigned int m_drawShader;

	float m_lastDrawTime;

};

struct GPUParticle {
		GPUParticle():lifetime(1),lifespan(0){}

		glm::vec3 position;
		glm::vec3 velocity;
		float lifetime;
		float lifespan;
};