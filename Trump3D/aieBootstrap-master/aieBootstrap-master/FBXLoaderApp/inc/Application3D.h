#include "FBXFile.h"
class GLMesh
{
public:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
};

#pragma once

#include "FBXFile.h"
#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Camera.h"

class MyCamera;
namespace aie {
	class Texture;
}
class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void CreateFBXOpenGLBuffers(FBXFile *file);
	void CleanupFBXOpenGLBuffers(FBXFile *file);

	void LoadShaders();
	void UnloadShaders();

	void CreateLandscape();
	void DrawLandscape();
	void DestroyLandscape();

protected:
	MyCamera *m_camera;
	aie::Texture *m_texture;
	aie::Texture *m_heightmap;


	unsigned int m_shader;

	unsigned int m_vertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	FBXFile *m_myFbxModel;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	// stats for my landscape
	const int m_landWidth = 200; // the number of vertices we're going to use to represent our landscape
	const int m_landLength = 200;
	const float m_vertSeparation = 0.1f;
	const float m_maxHeight = 2.0f;

	//light stats
	glm::vec3 m_lightPosition;
	glm::vec3 m_lightColour;
	glm::vec3 m_specularLight;
	float m_lightAmbientStrength;


	struct Vertex
	{
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec3 vNormal;

		static void SetupVertexAttribPointers();
	};

};