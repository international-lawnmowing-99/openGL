#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FBXFile.h"
#include <gl_core_4_4.h>
#include <Texture.h>


using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {

	m_myFbxModel = new FBXFile();
	m_myFbxModel->load("./models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER);
	CreateFBXOpenGLBuffers(m_myFbxModel);
	LoadShaders();

	m_camera = new MyCamera();
	m_camera->SetPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	m_camera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	m_texture = new aie::Texture();
	m_texture->load("textures/donaldtrump.png");
	m_heightmap = new aie::Texture();
	m_heightmap->load("textures/donaldtrumpHeightmap.bmp");

	m_lightPosition = glm::vec3(0.0f, 5.0f, 0.0f);
	m_lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
	m_specularLight = glm::vec3(0.9f, 0.5f, 0.2f);
	m_lightAmbientStrength = 0.1f;

	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//// create simple camera transforms
	//m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	//m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
	//									  getWindowWidth() / (float)getWindowHeight(),
	//									  0.1f, 1000.f);
	CreateLandscape();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void Application3D::shutdown() {

	Gizmos::destroy();
	UnloadShaders();
	CleanupFBXOpenGLBuffers(m_myFbxModel);
	m_myFbxModel->unload();
	DestroyLandscape();
	delete m_camera;
	delete m_myFbxModel;
}

void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	//// rotate camera
	//m_viewMatrix = glm::lookAt(vec3(glm::sin(time) * 10, 30, glm::cos(time) * 10),
	//						   vec3(0), vec3(0, 1, 0));

	//// wipe the gizmos clean for this frame
	//Gizmos::clear();

	m_camera->Update(deltaTime);

	// draw a simple grid with gizmos
	//vec4 white(1);
	//vec4 black(0, 0, 0, 1);
	//for (int i = 0; i < 21; ++i) {
	//	Gizmos::addLine(vec3(-10 + i, 0, 10),
	//					vec3(-10 + i, 0, -10),
	//					i == 10 ? white : black);
	//	Gizmos::addLine(vec3(10, 0, -10 + i),
	//					vec3(-10, 0, -10 + i),
	//					i == 10 ? white : black);
	//}

	//// add a transform so that we can see the axis
	//Gizmos::addTransform(mat4(1));

	// demonstrate a few shapes
	//Gizmos::addAABBFilled(vec3(0), vec3(1), vec4(0, 0.5f, 1, 0.25f));
	//Gizmos::addSphere(vec3(5, 0, 5), 1, 8, 8, vec4(1, 0, 0, 0.5f));
	//Gizmos::addRing(vec3(5, 0, -5), 1, 1.5f, 8, vec4(0, 1, 0, 1));
	//Gizmos::addDisk(vec3(-5, 0, 5), 1, 16, vec4(1, 1, 0, 1));
	//Gizmos::addArc(vec3(-5, 0, -5), 0, 2, 1, 8, vec4(1, 0, 1, 1));

	//mat4 t = glm::rotate(time, glm::normalize(vec3(1, 1, 1)));
	//t[3] = vec4(-2, 0, 0, 1);
	//Gizmos::addCylinderFilled(vec3(0), 0.5f, 1, 5, vec4(0, 1, 1, 1), &t);

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {


	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	// STEP 1: enable the shader program for rendering
	glUseProgram(m_shader);

	// Step 2: send uniform variables to the shader
	glm::mat4 projectionView = m_projectionMatrix * m_camera->GetView();
	glUniformMatrix4fv(
		glGetUniformLocation(m_shader, "projectionView"),
		1,
		false,
		glm::value_ptr(projectionView));

	glm::vec3 cameraPos = m_camera->GetPosition();
	glUniform3fv(glGetUniformLocation(m_shader, "cameraPos"), 1, &cameraPos[0]);



	// Set up Texture in OpenGL - select the first texture as active, then bind it
	// Also set it up as a uniform variable for the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getHandle());
	glUniform1i(glGetUniformLocation(m_shader, "texture"), 0);

	//set up light and colour info
	glUniform3fv(glGetUniformLocation(m_shader, "lightPosition"), 1, &m_lightPosition[0]);
	glUniform3fv(glGetUniformLocation(m_shader, "lightColour"), 1, &m_lightColour[0]);
	glUniform1fv(glGetUniformLocation(m_shader, "lightAmbientStrength"), 1, &m_lightAmbientStrength);
	glUniform3fv(glGetUniformLocation(m_shader, "specularLight"), 1, &m_specularLight[0]);

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


	Gizmos::draw(m_projectionMatrix * m_viewMatrix);

	float s = 1.0f;
	glm::mat4 model = glm::mat4(
		s, 0, 0, 0,
		0, s, 0, 0,
		0, 0, s, 0,
		0, 0, 0, 1
	);

	glm::mat4 modelViewProjection = m_projectionMatrix * m_viewMatrix * model;
	glUseProgram(m_shader);
	// send uniform variables, in this case the "projectionViewWorldMatrix"
	unsigned int mvpLoc = glGetUniformLocation(m_shader, "projectionViewWorldMatrix");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &modelViewProjection[0][0]);
	// loop through each mesh within the fbx file
	for (unsigned int i = 0; i < m_myFbxModel->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_myFbxModel->getMeshByIndex(i);
		GLMesh* glData = (GLMesh*)mesh->m_userData;
		// get the texture from the model
		unsigned int diffuseTexture = m_myFbxModel->getTextureByIndex(mesh->m_material->DiffuseTexture);
		// bind the texture and send it to our shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(glGetUniformLocation(m_shader, "diffuseTexture"), 0);
		// draw the mesh
		glBindVertexArray(glData->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

void Application3D::CreateFBXOpenGLBuffers(FBXFile *fbx)
{
	// FBX Files contain multiple meshes, each with seperate material information
	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
	// We can store that information within the mesh object via its "user data" void pointer variable.
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		// get the current mesh from file
		FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
		GLMesh *glData = new GLMesh();
		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);
		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);
		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
		// fill the vbo with our vertices.
		// the FBXLoader has convinently already defined a Vertex Structure for us.
		glBufferData(GL_ARRAY_BUFFER,
			fbxMesh->m_vertices.size() * sizeof(FBXVertex),
			fbxMesh->m_vertices.data(), GL_STATIC_DRAW);
		// fill the ibo with the indices.
		// fbx meshes can be large, so indices are stored as an unsigned int.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			fbxMesh->m_indices.size() * sizeof(unsigned int),
			fbxMesh->m_indices.data(), GL_STATIC_DRAW);
		// Setup Vertex Attrib pointers
		// remember, we only need to setup the approprate attributes for the shaders that will be rendering
		// this fbx object.
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // uv
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		// TODO: add any additional attribute pointers required for shader use.
		// unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// attach our GLMesh object to the m_userData pointer.
		fbxMesh->m_userData = glData;
	}
}
void Application3D::CleanupFBXOpenGLBuffers(FBXFile *fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		FBXMeshNode *fbxMesh = fbx->getMeshByIndex(i);
		GLMesh *glData = (GLMesh *)fbxMesh->m_userData;
		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);
		delete glData;
	}
}

void Application3D::LoadShaders()
{
//	const char* vertex_shader =
//		"#version 410\n \
//in vec4 position; \n\
//in vec4 normal; \n\
//in vec2 uv; \n\
//out vec4 vNormal; \n\
//out vec2 vuv; \n\
//uniform mat4 projectionViewWorldMatrix; \n\
//void main() { \n\
//vNormal = normal; \n\
//vuv = uv; \n\
//gl_Position = projectionViewWorldMatrix*position; \n\
//}";
//	const char* fragment_shader =
//		"#version 410\n \
//in vec4 vNormal; \n\
//in vec2 vuv; \n\
//out vec4 FragColor; \n\
//uniform sampler2D diffuseTexture; \n\
//void main() { \n\
//FragColor = texture2D(diffuseTexture, vuv) * vec4(1,1,1,1); \n\
//}";
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, (const char**)&vertex_shader, 0);
//	glCompileShader(vertexShader);
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, (const char**)&fragment_shader, 0);
//	glCompileShader(fragmentShader);
//	m_shader = glCreateProgram();
//	glAttachShader(m_shader, vertexShader);
//	glAttachShader(m_shader, fragmentShader);
//	glBindAttribLocation(m_shader, 0, "position");
//	glBindAttribLocation(m_shader, 1, "normal");
//	glBindAttribLocation(m_shader, 2, "uv");
//	glLinkProgram(m_shader);
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);

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

	static const char* fragment_shader =
		"#version 400\n																			\
	in vec2 fUv;\n																				\
	in vec3 fPos;\n																				\
	in vec4 fNormal;\n																			\
	uniform vec3 cameraPos;\n																	\
	float specularPower = 2.0f;\n																\
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
	  float specTerm = max(dot(reflect(cameraPos, norm),cameraPos - fPos),0.0f);\n				\
	  specTerm = pow(specTerm, specularPower);\n											\
	  vec3 specTermVec = (specTerm * specularLight) ;\n											\
	  vec3 ambient = lightColour * lightAmbientStrength;\n										\
	  frag_color = texture2D(texture, fUv) * vec4(ambient + difColour, 1.0f);\n		\
	}";

	// Step 1:
	// Load the vertex shader, provide it with the source code and compile it.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	
	// Step 2:
	// Load the fragment shader, provide it with the source code and compile it.
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	// step 3:
	// Create the shader program
	m_shader = glCreateProgram();

	// Step 4:
	// attach the vertex and fragment shaders to the m_shader program
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);

	// Step 5:
	// describe the location of the shader inputs the link the program
	glBindAttribLocation(m_shader, 0, "vPosition");
	glBindAttribLocation(m_shader, 1, "vUv");
	glBindAttribLocation(m_shader, 2, "vNormal");
	glLinkProgram(m_shader);

	// step 6:
	// delete the vs and fs shaders
	glDeleteShader(vs);
	glDeleteShader(fs);

}

void Application3D::Vertex::SetupVertexAttribPointers()
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

void Application3D::UnloadShaders()
{
	glDeleteProgram(m_shader);
}

void Application3D::CreateLandscape()
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
			float yPos = (pixels[i * 3] / 255.0f) * m_maxHeight; // *3 is so we sample only one colour from the RGB bitmap
			float zPos = (z * m_vertSeparation) - (m_landLength * m_vertSeparation * 0.5f);

			float u = (float)x / (m_landWidth - 1);
			float v = (float)z / (m_landLength - 1);


			Vertex vert{
				{ xPos, yPos, zPos, 1.0f },
				{ u, v },
				{ 0.0f, 0.0f, 0.0f }
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
			glm::vec3 Norm = glm::cross(glm::vec3(verts.at(i + m_landWidth).pos - verts.at(i).pos), glm::vec3(verts.at(i + 1).pos - verts.at(i).pos));
			//all points on a flat triangle will have the same normal duh
			verts.at(i).vNormal += Norm;
			verts.at(i + 1).vNormal += Norm;
			verts.at(i + m_landWidth).vNormal += Norm;


			indices.push_back(i + 1);				//     a
			indices.push_back(i + m_landWidth);		//   / |
			indices.push_back(i + m_landWidth + 1); //  b--c

													// Normals for this triangle
													//  a-c X b-c
			glm::vec3 SecondNorm = glm::cross(glm::vec3(verts.at(i + 1).pos - verts.at(i + m_landWidth + 1).pos), glm::vec3(verts.at(i + m_landWidth).pos - verts.at(i + m_landWidth + 1).pos));
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

void Application3D::DrawLandscape()
{
	glDrawElements(GL_TRIANGLES, m_IndicesCount, GL_UNSIGNED_SHORT, 0);
}

void Application3D::DestroyLandscape()
{
	glDeleteBuffers(1, &m_Ibo);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_Vao);

}
