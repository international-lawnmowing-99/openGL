#include "GPUParticleEmitter.h"
#include <gl_core_4_4.h>
#include <glm\mat4x4.hpp>

GPUParticleEmitter::GPUParticleEmitter(): m_particles(nullptr), m_maxParticles(0), m_position(0,0,0), m_drawShader(0), m_updateShader(0), m_lastDrawTime(0)
{
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}

void GPUParticleEmitter::Initialise(unsigned int maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, const glm::vec4 & startColour, const glm::vec4 & endColour)
{
	m_startColour = startColour;
	m_endColour = endColour;
	m_startSize = startSize;
	m_endSize = endSize;
	m_velocityMax = velocityMax;
	m_velocityMin = velocityMin;
	m_lifespanMax = lifespanMax;
	m_lifespanMin = lifespanMin;
	m_maxParticles = maxParticles;

	m_particles = new GPUParticle[maxParticles];


	m_activeBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUParticleEmitter::Draw(float time,  const glm::mat4&  cameraTransform, const glm::mat4& projectionView)
{
	glUseProgram(m_updateShader);
	int location = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(location, time);
	float deltaTime = time - m_lastDrawTime;
	m_lastDrawTime = time;

	location = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(location, deltaTime);
	location = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(location, 1, &m_position[0]);
	glEnable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(m_vao[m_activeBuffer]);
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glUseProgram(m_drawShader);
	location = glGetUniformLocation(m_drawShader, "projectionView");
	glUniformMatrix4fv(location, 1, false, &projectionView[0][0]);
	location = glGetUniformLocation(m_drawShader, "cameraTransform");
	glUniformMatrix4fv(location, 1, false, &cameraTransform[0][0]);
	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);
	m_activeBuffer = otherBuffer;

}

void GPUParticleEmitter::CreateBuffers() {
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticleEmitter::CreateUpdateShader()
{
	const char * vsText = "#version 410\
layout(location = 0) in vec3 Position;\
layout(location = 1) in vec3 Velocity;\
layout(location = 2) in float Lifetime;\
layout(location = 3) in float Lifespan;\
\
out vec3 position;\
out vec3 velocity;\
out float lifetime;\
out float lifespan;\
\
uniform float time;\
uniform float deltaTime;\
uniform float lifeMin;\
uniform float lifeMax;\
uniform vec3 emitterPosition;\
\
const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f; \
float rand(uint seed, float range) { \
uint i = (seed ^ 12345391u) * 2654435769u; \
i ^= (i << 6u) ^ (i >> 26u); \
i *= 2654435769u; \
i += (i << 5u) ^ (i >> 12u); \
return float(range * i) * INVERSE_MAX_UINT;\
}\
void main() {\
position = Position + Velocity * deltaTime;\
velocity = Velocity;\
lifetime = Lifetime + deltaTime;\
lifespan = LifeSpan;\
\
if(lifetime > lifespan)\
{\
uint seed = uion(time * 1000.0) + uint(gl_VertexID);\
velocity.x = rand(seed++, 2) - 1;\
velocity.y = rand(seed++, 2) - 1;\
velocity.z = rand(seed++, 2) - 1;\
velocity = normalize(velocity);\
position = emitterPosition;\
lifetime = 0;\
lifespan = rand(seed++, lifeMax - lifeMin) + lifeMin;\
}\
}";
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsText, NULL);
	glCompileShader(vs);

	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);
	const char * varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_updateShader);
	glDeleteShader(vs);
	glUseProgram(m_updateShader);
	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);
	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax);
}


void GPUParticleEmitter::CreateDrawShader()
{
	const char * vsText = "#version 410\
layout(location=0) in vec3 Position;\
layout(location=1) in vec3 Velocity;\
layout(location=2) in float Lifetime;\
layout(location=3) in float Lifespan;\
\
out vec3 position;\
out float lifetime;\
out float lifespan;\
\
void main(){\
position = Position;\
lifetime = Lifetime;\
lifespan = Lifespan;\
}";

	const char * gsText = "#version 410\
layout(points) in;\
layout(triangle_strip, max_vertices = 4) out;\
in vec3 position[];\
in float lifetime[];\
in float lifespan[];\
\
out vec4 Colour;\
\
uniform mat4 projectionView;\
unform mat4 cameraTransform;\
\
uniform float sizeStart;\
uniform float sizeEnd;\
\
uniform vec4 colourStart;\
uniform vec4 colourEnd;\
\
void main(){\
Colour = mix(colourStart, colourEnd, lifetime[0] / lifespan[0]);\
float halfSize = mix(sizeStart, sizeEnd, lifetime[0]/lifespan[0]) * 0.5f;\
\
vec3 corners[4];\
corners[0] = vec3(halfSize, -halfSize, 0);\
corners[1] = vec3(halfSize, halfSize, 0);\
corners[2] = vec3(-halfSize, -halfSize, 0);\
corners[3] = vec3(-halfSize, halfSize, 0);\
\
vec3 zAxis = normalize(cameraTransform[3].xyz - position[0]);\
vec3 xAxis = cross(cameraTransform[1].xyz, zAxis);\
vec3 yAxis = cross(zAxis, xAxis);\
mat3 billboard = mat3(xAxis,yAxis,zAxis);\
\
gl_Position = projectionView*vec4(billboard*corners[0]+position[0], 1);\
EmitVertex();\
gl_Position = projectionView*vec4(billboard*corners[1]+position[0], 1);\
EmitVertex();\
gl_Position = projectionView*vec4(billboard*corners[2]+position[0], 1);\
EmitVertex();\
gl_Position = projectionView*vec4(billboard*corners[3]+position[0], 1);\
EmitVertex();";

	const char * fsText = "#version 410\
in vec4 Colour;\
out vec4 fragColour;\
void main(){\
fragColour = Colour;}";


	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsText, NULL);
	glCompileShader(vs);

	unsigned int gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &gsText, NULL);
	glCompileShader(gs);

	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsText, NULL);
	glCompileShader(fs);

	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glAttachShader(m_drawShader, gs);
	glLinkProgram(m_drawShader);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	glUseProgram(m_drawShader);

	int location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);
	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	location = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location, 1, &m_startColour[0]);
	location = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location, 1, &m_endColour[0]);
}
