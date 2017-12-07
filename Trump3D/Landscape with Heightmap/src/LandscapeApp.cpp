

#include "LandscapeApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include <imgui.h>
#include <Texture.h>
#include "gl_core_4_4.h"
#include "glfw3.h" 
#include <FBXFile.h>


using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

LandscapeApp::LandscapeApp() {

}

LandscapeApp::~LandscapeApp() {

}

bool LandscapeApp::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);


	m_soulspearPosition = glm::mat4(
		1, 0, 0, 10,
		0, 1, 0, 10,
		0, 0, 1, 10,
		0, 0, 0, 1
	);

	// Set up camera starting position and where it's looking
	m_camera = new MyCamera(this);
	m_camera->SetPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	m_camera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	//setup light 
	m_lightPosition = glm::vec3(0.0f, 5.0f, 0.0f);
	m_lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
	m_specularLight = glm::vec3(0.9f, 0.5f, 0.2f);
	m_lightAmbientStrength = 0.1f;

	// load texture and heightmap
	m_texture = new aie::Texture();
	m_texture->load("textures/donaldtrump.png");
	m_heightmap = new aie::Texture();
	m_heightmap->load("textures/donaldtrumpHeightmap.bmp");

	//Load the soulspear
	m_myFbxModel = new FBXFile();
	m_myFbxModel->load("./models/soulspear/soulspear.fbx", FBXFile::UNITS_DECIMETER, true, true);
	CreateFBXOpenGLBuffers(m_myFbxModel);

	LoadShader();
	CreateLandscape();

	// Attempt to greate gpu particle system
	m_emitter = new GPUParticleEmitter();
	m_emitter->Initialise(100000, 0.1f, 5.0f, 5, 20, 1, 0.9f, glm::vec4(1), glm::vec4(1, 12, 1, 1));

	//setup blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void LandscapeApp::shutdown() {
	DestroyLandscape();
	UnloadShader();

	Gizmos::destroy();
	delete m_camera;
}

void LandscapeApp::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	// Control Camera
	m_camera->Update(deltaTime);

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	//Rotate the light like a sun and draw a gizmo on its position
	Gizmos::addSphere(m_lightPosition, .4f, 32, 32, vec4(0, 0, 0, 0.5f));
	m_lightPosition = glm::vec3(15.0 * cos(time), 10.0 , 15.0 * sin(time));

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	bool visible = m_camera->isVisible(m_lightPosition, glm::vec3(m_lightPosition - glm::vec3(0.0, 0.0, 0.4)));

	if (visible)
	{
		ImGui::Text("Sphere Visible");
	}
	else
		ImGui::Text("Spere is not visible");

}

void LandscapeApp::DrawGrid()
{
	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
}

void LandscapeApp::draw() {
	
	// wipe the screen to the background colour
	clearScreen();
	//set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	//attempt to draw particles
	glm::vec4 cameraVec4pos = glm::vec4(m_camera->GetPosition(), 0);
	glm::mat4 cameraTransform = glm::mat4(glm::vec4(1), glm::vec4(1), glm::vec4(1), cameraVec4pos);
	m_emitter->Draw((float)glfwGetTime(), cameraTransform, m_camera->GetView());

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	// STEP 1: enable the shader program for rendering
	glUseProgram(m_trumpShader);

	// Step 2: send uniform variables to the shader
	glm::mat4 projectionView = m_projectionMatrix * m_camera->GetView();
	glUniformMatrix4fv(
		glGetUniformLocation(m_trumpShader, "projectionView"), 
		1, 
		false, 
		glm::value_ptr(projectionView));

	glm::vec3 cameraPos = m_camera->GetPosition();
	glUniform3fv(glGetUniformLocation(m_trumpShader, "cameraPos"), 1, &cameraPos[0]);

	// Set up Texture in OpenGL - select the first texture as active, then bind it
	// Also set it up as a uniform variable for the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getHandle());
	glUniform1i(glGetUniformLocation(m_trumpShader, "texture"), 0);

	//set up light and colour info
	glUniform3fv(glGetUniformLocation(m_trumpShader, "lightPosition"), 1, &m_lightPosition[0]);
	glUniform3fv(glGetUniformLocation(m_trumpShader, "lightColour"), 1, &m_lightColour[0]);
	glUniform1fv(glGetUniformLocation(m_trumpShader, "lightAmbientStrength"), 1, &m_lightAmbientStrength);
	glUniform3fv(glGetUniformLocation(m_trumpShader, "specularLight"), 1, &m_specularLight[0]);

	// Step 3: Bind the VAO
	// When we setup the geometry, we did a bunch of glEnableVertexAttribArray and glVertexAttribPointer method calls
	// we also Bound the vertex array and index array via the glBindBuffer call.
	// if we where not using VAO's we would have to do thoes method calls each frame here.
	glBindVertexArray(m_Vao);

	// Step 4: Draw Elements. We are using GL_TRIANGLES.
	// we need to tell openGL how many indices there are, and the size of our indices
	// when we setup the geometry, our indices were an unsigned short
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_SHORT, 0);
	//DrawLandscape();

	// Step 5: Now that we are done drawing the geometry
	// unbind the vao, we are basicly cleaning the opengl state
	glBindVertexArray(0);

	// Step 6: de-activate the shader program, dont do future rendering with it any more.
	glUseProgram(0);

	Gizmos::draw(m_projectionMatrix * m_camera->GetView());

	//optional draw the grid
	//DrawGrid();

	//draw the model
	float m_scale = 1.0f;
	
	glm::mat4 m_scaleMatrix = glm::mat4(
		m_scale, 0, 0, 0,
		0, m_scale, 0, 0,
		0, 0, m_scale, 0,
		0, 0, 0, 1
	);

	glm::mat4 modelViewProjection =  m_projectionMatrix *m_camera->GetView() * m_scaleMatrix;

	glUseProgram(m_trumpShader);
	/*
	TODO: soulspear shader
	*/

	unsigned int mvpLoc = glGetUniformLocation(m_trumpShader, "projectionView");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &modelViewProjection[0][0]);


	for (unsigned int i = 0; i < 10; i++)
	{
		for (unsigned int i = 0; i < m_myFbxModel->getMeshCount(); i++)
		{
			FBXMeshNode* mesh = m_myFbxModel->getMeshByIndex(i);
			GLMesh* glData = (GLMesh*)mesh->m_userData;
			
			unsigned int diffuseTexture = m_myFbxModel->getTextureByIndex(mesh->m_material->DiffuseTexture);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseTexture);
			glUniform1i(glGetUniformLocation(m_soulspearShader, "texture"), 0);

			glBindVertexArray(glData->vao);
			glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	glUseProgram(0);
}


void LandscapeApp::LoadShader()
{
	static const char* vertex_shader =
		"#version 400\n																\
	in vec4 vPosition;\n															\
	in vec2 vUv;\n																	\
	in vec4 vNormal;\n																\
	out vec2 fUv;\n																	\
	out vec3 fPos;\n																\
	out vec4 fNormal;\n																\
	uniform mat4 projectionView; \n													\
	void main ()\n																	\
	{\n																				\
	  fPos = vPosition.xyz;\n														\
	  fNormal = vNormal;\n															\
	  fUv = vUv;\n																	\
	  gl_Position = projectionView * vPosition;\n									\
	}";

	static const char* soulspear_vertex_shader =
		"#version 400\n																\
	in vec4 vPosition;\n															\
	in vec2 vUv;\n																	\
	in vec4 vNormal;\n																\
	out vec2 fUv;\n																	\
	out vec3 fPos;\n																\
	out vec4 fNormal;\n																\
	uniform mat4 projectionViewWorldMatrix; \n													\
	void main ()\n																	\
	{\n																				\
	  fPos = vPosition.xyz;\n														\
	  fNormal = vNormal;\n															\
	  fUv = vUv;\n																	\
	  gl_Position = projectionViewWorldMatrix * vPosition;\n									\
	}";

	static const char* fragment_shader =
		"#version 400\n																			\
	in vec2 fUv;\n																				\
	in vec3 fPos;\n																				\
	in vec4 fNormal;\n																			\
	uniform vec3 cameraPos;\n																	\
	float specularPower = 64.0f;\n																\
	out vec4 frag_color;\n																		\
	uniform sampler2D texture;\n																\
	uniform float lightAmbientStrength;\n														\
	uniform vec3 specularLight;\n																\
	uniform vec3 lightPosition;\n																\
	uniform vec3 lightColour;\n																	\
	void main ()\n																				\
	{\n																							\
	  vec3 norm = normalize(fNormal.xyz);\n														\
	  vec3 lightDir = normalize(  lightPosition - fPos);\n										\
	  float dif = max(dot(norm, lightDir), 0.0f);\n												\
	  vec3 difColour = dif * lightColour;\n														\
	  float specTerm = max(dot(normalize(reflect(cameraPos, norm)),normalize(cameraPos - lightPosition)),0.0f);\n				\
	  float specTerm2 = (pow(specTerm, specularPower));\n										\
	  vec3 specTerm3 = ((specTerm2 * specularLight)) ;\n								\
	  vec3 ambient = lightColour * lightAmbientStrength;\n										\
	  frag_color = texture2D(texture, fUv) * vec4(ambient + difColour + specTerm3, 1.0f);\n		\
	}";

	static const char* soulspear_fragment_shader =
		"#version 400\n																			\
	in vec2 fUv;\n																				\
	in vec3 fPos;\n																				\
	in vec4 fNormal;\n																			\
	uniform vec3 cameraPos;\n																	\
	float specularPower = 64.0f;\n																\
	out vec4 frag_color;\n																		\
	uniform sampler2D texture;\n																\
	uniform float lightAmbientStrength;\n														\
	uniform vec3 specularLight;\n																\
	uniform vec3 lightPosition;\n																\
	uniform vec3 lightColour;\n																	\
	void main ()\n																				\
	{\n																							\
	  vec3 norm = normalize(fNormal.xyz);\n														\
	  vec3 lightDir = normalize(  lightPosition - fPos);\n										\
	  float dif = max(dot(norm, lightDir), 0.0f);\n												\
	  vec3 difColour = dif * lightColour;\n														\
	  float specTerm = max(dot(normalize(reflect(cameraPos, norm)),normalize(cameraPos - lightPosition)),0.0f);\n				\
	  float specTerm2 = (pow(specTerm, specularPower));\n										\
	  vec3 specTerm3 = ((specTerm2 * specularLight)) ;\n								\
	  vec3 ambient = lightColour * lightAmbientStrength;\n										\
	  frag_color = vec4(0.8f,0.8f,0.8f,1.0f) ;\n		\
	}";

	// Step 1:
	// Load the vertex shader, provide it with the source code and compile it.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);

	GLuint soulspearVs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(soulspearVs, 1, &soulspear_vertex_shader, NULL);
	glCompileShader(soulspearVs);

	// Step 2:
	// Load the fragment shader, provide it with the source code and compile it.
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	GLuint soulspearFs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(soulspearFs, 1, &soulspear_fragment_shader, NULL);
	glCompileShader(soulspearFs);

	// step 3:
	// Create the shader program
	m_trumpShader = glCreateProgram();
	m_soulspearShader = glCreateProgram();

	// Step 4:
	// attach the vertex and fragment shaders to the m_shader program
	glAttachShader(m_trumpShader, vs);
	glAttachShader(m_trumpShader, fs);

	glAttachShader(m_soulspearShader, soulspearVs);
	glAttachShader(m_soulspearShader, soulspearFs);

	// Step 5:
	// describe the location of the shader inputs the link the program
	glBindAttribLocation(m_trumpShader, 0, "vPosition");
	glBindAttribLocation(m_trumpShader, 1, "vUv");
	glBindAttribLocation(m_trumpShader, 2, "vNormal");
	glLinkProgram(m_trumpShader);

	glBindAttribLocation(m_soulspearShader, 0, "vPosition");
	glBindAttribLocation(m_soulspearShader, 1, "vUv");
	glBindAttribLocation(m_soulspearShader, 2, "vNormal");
	glLinkProgram(m_soulspearShader);

	// step 6:
	// delete the vs and fs shaders
	glDeleteShader(vs);
	glDeleteShader(fs);

	glDeleteShader(soulspearVs);
	glDeleteShader(soulspearFs);
}

void LandscapeApp::UnloadShader()
{
	glDeleteProgram(m_trumpShader);
	glDeleteProgram(m_soulspearShader);
}

void LandscapeApp::Vertex::SetupVertexAttribPointers()
{
	// enable vertex position element
	// notice when we loaded the shader, we described the "position" element to be location 0.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0 (position)
		4,                  // size - how many floats make up the position (x, y, z, w)
		GL_FLOAT,           // type - our x,y,z, w are float values
		GL_FALSE,           // normalised? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)0            // offset - bytes from the beginning of the vertex
	);

	// enable vertex uv element
	// notice when we loaded the shader, we described the uv element to be location 1.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 1 (colour)
		2,                  // size - how many floats make up the colour (r, g, b, a)
		GL_FLOAT,           // type - our x,y,z are float values
		GL_FALSE,           // normalised? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)(sizeof(float) * 4)            // offset - bytes from the beginning of the vertex
	);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute 2 (normal)
		4,                  // size - how many floats make up the normal (x, y, z, w)
		GL_FLOAT,           // type - our x,y,z are float values
		GL_FALSE,           // normalised? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)(sizeof(float) * 6)            // offset - bytes from the beginning of the vertex
	);
}

void LandscapeApp::CreateLandscape()
{
	std::vector<Vertex> verts;
	std::vector<unsigned short> indices;

	const unsigned char *pixels = m_heightmap->getPixels();

	// create a grid of Vertices
	for (int z = 0; z < m_landLength; z++)
	{
		for (int x = 0; x < m_landWidth; x++)
		{
			// sample from a pixel in the heightmap that corresponds
			// to the x and z position
			int sampleX = (int)((float)x / m_landWidth * m_heightmap->getWidth());
			int sampleZ = (int)((float)z / m_landLength * m_heightmap->getHeight());

			int i = sampleZ * m_heightmap->getWidth() + sampleX; // access array like it's a 2 dimensional array

			// position of vertex
			float xPos = (x * m_vertSeparation) - (m_landWidth * m_vertSeparation * 0.5f);
			float yPos = (pixels[i*3] / 255.0f) * m_maxHeight; // *3 is so we sample only one colour from the RGB bitmap
			float zPos = (z * m_vertSeparation) - (m_landLength * m_vertSeparation * 0.5f);

			float u = (float)x / (m_landWidth - 1);
			float v = (float)z / (m_landLength - 1);


			Vertex vert {
				{ xPos, yPos, zPos, 1.0f },
				{ u, v },
				{ 0.0f, 0.0f, 0.0f, 0.0f}
			};
			verts.push_back(vert);
		}
	}

	// calculate indices for triangles
	for (int z = 0; z < m_landLength - 1; z++)
	{
		for (int x = 0; x < m_landWidth - 1; x++)
		{
			int i = z * m_landWidth + x; // the address of the vertices in the single dimension vector

			// triangulate!
			indices.push_back(i + 1);				//  b--a
			indices.push_back(i);					//  | /
			indices.push_back(i + m_landWidth);		//  c

			// Normals for this triangle
			//										c-b															a-b
			glm::vec4 Norm = glm::vec4(glm::cross(glm::vec3(verts.at(i + m_landWidth).pos - verts.at(i).pos), glm::vec3(verts.at(i + 1).pos - verts.at(i).pos)),0.0f);
			//all points on a flat triangle will have the same normal duh
			verts.at(i).vNormal += Norm;
			verts.at(i + 1).vNormal += Norm;
			verts.at(i + m_landWidth).vNormal += Norm;


			indices.push_back(i + 1);				//     a
			indices.push_back(i + m_landWidth);		//   / |
			indices.push_back(i + m_landWidth + 1); //  b--c
				
			// Normals for this triangle
			//  a-c X b-c
			glm::vec4 SecondNorm = glm::vec4(glm::cross(glm::vec3(verts.at(i+1).pos - verts.at(i+m_landWidth + 1).pos),glm::vec3(verts.at(i + m_landWidth).pos - verts.at(i + m_landWidth + 1).pos)),0.0f);
			verts.at(i + 1).vNormal += SecondNorm;
			verts.at(i + m_landWidth).vNormal += SecondNorm;
			verts.at(i + m_landWidth + 1).vNormal += SecondNorm;
		}
	}

	m_vertCount = verts.size();
	m_IndicesCount = indices.size();

	for (Vertex &vert : verts)
	{
		glm::normalize(vert.vNormal);
	}
	//calculate the normal vectors for each vertex


	// Generate the VAO and Bind bind it.
	// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
	// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	// Create our VBO and IBO.
	// Then tell Opengl what type of buffer they are used for
	// VBO a buffer in graphics memory to contains our vertices
	// IBO a buffer in graphics memory to contain our indices.
	// Then Fill the buffers with our generated data.
	// This is taking our verts and indices from ram, and sending them to the graphics card
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ibo);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);

	glBufferData(GL_ARRAY_BUFFER, m_vertCount * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesCount * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	Vertex::SetupVertexAttribPointers();

	// unbind things after we've finished using them
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LandscapeApp::DestroyLandscape()
{
	// When We're Done, destroy the geometry
	glDeleteBuffers(1, &m_Ibo);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_Vao);
}

glm::mat4 LandscapeApp::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

glm::mat4 LandscapeApp::GetViewMatrix()
{
	return m_viewMatrix;
}

void LandscapeApp::CleanUpFBXOpenGLBuffers(FBXFile * file)
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

void LandscapeApp::CreateFBXOpenGLBuffers(FBXFile * file)
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