#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "GPUParticleEmitter.h"
#include "FBXModel.h"
#include <Camera.h>
#include <FBXFile.h>

class MyCamera;
namespace aie
{
	class Texture;
}


class LandscapeApp : public aie::Application {
public:

	LandscapeApp();
	virtual ~LandscapeApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void DrawGrid();

	void LoadShader();
	void UnloadShader();

	void CreateLandscape();
	void DestroyLandscape();

	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();

protected:

	void CreateFBXOpenGLBuffers(FBXFile* file);
	void CleanUpFBXOpenGLBuffers(FBXFile* file);

	FBXFile * m_myFbxModel;

	MyCamera *m_camera;
	aie::Texture *m_texture;
	aie::Texture *m_heightmap;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_soulspearPosition;

	GPUParticleEmitter *m_emitter;
	
	unsigned int m_trumpShader;
	unsigned int m_soulspearShader;


	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	// stats for my landscape
	const int m_landWidth = 200; // the number of vertices we're going to use to represent our landscape
	const int m_landLength = 200;
	const float m_vertSeparation = 0.1f;
	const float m_maxHeight = 3.0f;

	//light stats
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColour;
	glm::vec3 m_specularLight;
	float m_lightAmbientStrength;

	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec4 vNormal;

		static void SetupVertexAttribPointers();
	};
};