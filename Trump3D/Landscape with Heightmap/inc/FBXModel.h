#pragma once

#include "FBXFile.h"


class GLMesh
{
public:
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
};

class LandscapeApp;
class FBXModel
{
public:
	FBXModel(LandscapeApp* parent);
	~FBXModel();

	void CreateFBXOpenGLBuffers(FBXFile* file);
	void CleanUpFBXOpenGLBuffers(FBXFile* file);

	void LoadShaders();
	void UnloadShaders();

	void Draw();

	unsigned int m_shader;

	FBXFile * m_fbxModel;
private:
protected:
	LandscapeApp* app;
	float m_scale = 1.0f;
	glm::mat4 m_scaleMatrix = glm::mat4(
		m_scale, 0,	0, 0,
		0, m_scale, 0, 0,
		0, 0, m_scale, 0,
		0, 0, 0, 1
	);

};

