#include "FBXModel.h"
#include <gl_core_4_4.h>
#include <LandscapeApp.h>

FBXModel::FBXModel(LandscapeApp* parent)
{
	app = parent;
	m_fbxModel = new FBXFile();
	m_fbxModel->load("./models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER);
	CreateFBXOpenGLBuffers(m_fbxModel);
	LoadShaders();
}

FBXModel::~FBXModel()
{
	UnloadShaders();
	CleanUpFBXOpenGLBuffers(m_fbxModel);
	m_fbxModel->unload();
	delete m_fbxModel;
}

void FBXModel::CreateFBXOpenGLBuffers(FBXFile * file)
{
	for (unsigned int i = 0; i < file->getMeshCount(); i++)
	{
		FBXMeshNode *fbxMesh = file->getMeshByIndex(i);
		GLMesh *glData = new GLMesh();
		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);

		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);

		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);

		glBufferData(GL_ARRAY_BUFFER, fbxMesh->m_vertices.size() * sizeof(FBXVertex), fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, fbxMesh->m_indices.size() * sizeof(unsigned int), fbxMesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		fbxMesh->m_userData = glData;
	}
}

void FBXModel::CleanUpFBXOpenGLBuffers(FBXFile * file)
{
	for (unsigned int i = 0; i < file->getMeshCount(); i++)
	{
		FBXMeshNode * fbxMesh = file->getMeshByIndex(i);
		GLMesh *glData = (GLMesh *)fbxMesh->m_userData;

		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->ibo);
		glDeleteBuffers(1, &glData->vbo);

		delete glData;
	}
}

void FBXModel::LoadShaders()
{
	const char* vsSource =
		"#version 410\n \
		in vec4 position; \n\
		in vec4 normal; \n\
		in vec2 uv; \n\
		out vec4 vNormal; \n\
		out vec2 vuv; \n\
		uniform mat4 projectionViewWorldMatrix; \n\
		void main() {		\n\
			vNormal = normal; \n\
			vuv = uv; \n\
			gl_Position = projectionViewWorldMatrix*position; \n\
	}";

	const char * fsSource =
		"#version 410\n \
		in vec4 vNormal; \n\
		in vec2 vuv; \n\
		out vec4 FragColor; \n\
		uniform sampler2D diffuseTexture; \n\
		void main() {		\n\
			FragColor = texture2D(diffuseTexture, vuv) * vec4(1, 1, 1, 1); \n\
	}";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_shader = glCreateProgram();

	glAttachShader(m_shader, vertexShader);
	glAttachShader(m_shader, fragmentShader);

	glBindAttribLocation(m_shader, 0, "position");
	glBindAttribLocation(m_shader, 1, "normal");
	glBindAttribLocation(m_shader, 2, "uv");

	glLinkProgram(m_shader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void FBXModel::UnloadShaders()
{
	glDeleteProgram(m_shader);
}

void FBXModel::Draw()
{
	glm::mat4 modelViewProjection = app->GetProjectionMatrix() * app->GetViewMatrix() * m_scaleMatrix;

	glUseProgram(m_shader);

	unsigned int mvpLoc = glGetUniformLocation(m_shader, "profectionViewWorldMatrix");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &modelViewProjection[0][0]);

	for (unsigned int i = 0; i < m_fbxModel->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = m_fbxModel->getMeshByIndex(i);
		GLMesh* glData = (GLMesh*)mesh->m_userData;

		unsigned int diffuseTexture = m_fbxModel->getTextureByIndex(mesh->m_material->DiffuseTexture);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(glGetUniformLocation(m_shader, "diffuseTexture"), 0);

		glBindVertexArray(glData->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}