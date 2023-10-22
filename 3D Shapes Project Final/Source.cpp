// CS-330 Module 4-5
// Sept 24, 2023
//
// Based on example code provided by SNHU instructor Brian Battersby


#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "image.h"
#include "Header.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "3D Shapes Project"; // Macro for window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 2000;
	const int WINDOW_HEIGHT = 1500;

	// Stores the GL data relative to a given mesh
	struct GLMesh
	{
		GLuint vao;         // Handle for the vertex array object
		GLuint vbos[2];     // Handles for the vertex buffer objects
		GLuint nVertices;	// Number of vertices for the mesh
		GLuint nIndices;    // Number of indices for the mesh
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;

	// Texture ID
	GLuint gTextureWood;
	GLuint gTextureDesk;
	GLuint gTextureMug;
	GLuint gTextureChemex;
	glm::vec2 gUVScale(2.0f, 2.0f);
	GLint gTexWrapMode = GL_REPEAT;

	// Shader program
	GLuint gSurfaceProgramId;
	GLuint gLightProgramId;

	// camera
	Camera gCamera(glm::vec3(0.0f, 7.0f, 15.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// timing
	float gDeltaTime = 0.0f; // time between current frame and last frame
	float gLastFrame = 0.0f;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */

GLMesh gPlaneMesh;
GLMesh gBoxUpperMesh;
GLMesh gBoxLowerMesh;
GLMesh gPerfumeMesh;
GLMesh gMugMesh;
GLMesh gUpperChemexMesh;
GLMesh gLowerChemexMesh;
GLMesh gChemexCollar;
GLMesh gCapMesh;
GLMesh gHandleMesh;
GLMesh gWallMesh;

bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CreateMeshes();
void DestroyMeshes();
void UCreatePlaneMesh(GLMesh& mesh);
void UCreateBoxMesh(GLMesh& mesh);
void UCreateCylinderMesh(GLMesh& mesh);
void UCreateConeMesh(GLMesh& mesh);
void UCreateTorusMesh(GLMesh& mesh);
void UCreateHalfTorusMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

bool perspective = false;

// function to creat meshes
void CreateMeshes() {
	UCreatePlaneMesh(gPlaneMesh);
	UCreateBoxMesh(gBoxUpperMesh);
	UCreateBoxMesh(gBoxLowerMesh);
	UCreateBoxMesh(gPerfumeMesh);
	UCreateCylinderMesh(gMugMesh);
	UCreateConeMesh(gUpperChemexMesh);
	UCreateConeMesh(gLowerChemexMesh);
	UCreateTorusMesh(gChemexCollar);
	UCreateCylinderMesh(gCapMesh);
	UCreateHalfTorusMesh(gHandleMesh);
}

// function to destroy meshes
void DestroyMeshes() {
	UDestroyMesh(gBoxUpperMesh);
	UDestroyMesh(gBoxLowerMesh);
	UDestroyMesh(gPerfumeMesh);
	UDestroyMesh(gPlaneMesh);
	UDestroyMesh(gMugMesh);
	UDestroyMesh(gUpperChemexMesh);
	UDestroyMesh(gLowerChemexMesh);
	UDestroyMesh(gChemexCollar);
	UDestroyMesh(gCapMesh);
	UDestroyMesh(gHandleMesh);
}

// function to destroy individual mesh
void UDestroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vao);
	glDeleteBuffers(2, mesh.vbos);
}

//Create plane for objects to rest on
void UCreatePlaneMesh(GLMesh& mesh)
{
	// Vertex data
	GLfloat verts[] = {
		// Vertex Positions		// Normals			// Texture coords	// Index
		-1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,			// bottom left
		1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,			// bottom right
		1.0f,  0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,			//2
		-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,			//3
	};

	// Index data
	GLuint indices[] = {
		0,1,2,
		0,3,2
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

	// Generate the VAO for the mesh
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);	// activate the VAO

	// Create VBOs for the mesh
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends data to the GPU

	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); // Activates the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void UCreateBoxMesh(GLMesh& mesh)
{
	// Position and Color data
	GLfloat verts[] = {
		//Positions				//Normals
		// ------------------------------------------------------

		//Back Face				//Negative Z Normal  Texture Coords.
		1.5f, 0.25f, -1.5f,		0.0f,  0.0f, -1.0f,  0.0f, 1.0f,   //top right 0
		1.5f, -0.25f, -1.5f,		0.0f,  0.0f, -1.0f,  0.0f, 0.0f,   //bottom right 1
		-1.5f, -0.25f, -1.5f,	0.0f,  0.0f, -1.0f,  1.0f, 0.0f,   //top left 2
		-1.5f, 0.25f, -1.5f,		0.0f,  0.0f, -1.0f,  1.0f, 1.0f,   //bottom left 3

		//Bottom Face			//Negative Y Normal
		-1.5f, -0.25f, 1.5f,		0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  //4
		-1.5f, -0.25f, -1.5f,	0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  //5
		1.5f, -0.25f, -1.5f,		0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  //6
		1.5f, -0.25f,  1.5f,		0.0f, -1.0f,  0.0f,  1.0f, 1.0f, //7

		//Left Face				//Negative X Normal
		-1.5f, 0.25f, -1.5f,		1.0f,  0.0f,  0.0f,  0.0f, 1.0f,      //8
		-1.5f, -0.25f,  -1.5f,	1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  //9
		-1.5f,  -0.25f,  1.5f,	1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  //10
		-1.5f,  0.25f,  1.5f,	1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  //11

		//Right Face			//Positive X Normal
		1.5f,  0.25f,  1.5f,		1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  //12
		1.5f,  -0.25f, 1.5f,		1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  //13
		1.5f, -0.25f, -1.5f,		1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  //14
		1.5f, 0.25f, -1.5f,		1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  //15

		//Top Face				//Positive Y Normal
		-1.5f,  0.25f, -1.5f,	0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //16
		-1.5f,  0.25f, 1.5f,		0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //17
		1.5f,  0.25f,  1.5f,		0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //18
		1.5f,  0.25f,  -1.5f,	0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //19

		//Front Face			//Positive Z Normal
		-1.5f, 0.25f,  1.5f,	    0.0f,  0.0f,  1.0f,  0.0f, 1.0f, // top right 20
		-1.5f, -0.25f,  1.5f,	0.0f,  0.0f,  1.0f,  0.0f, 0.0f, // bottom right 21
		1.5f,  -0.25f,  1.5f,	0.0f,  0.0f,  1.0f,  1.0f, 0.0f, // top left 22
		1.5f,  0.25f,  1.5f,		0.0f,  0.0f,  1.0f,  1.0f, 1.0f, // bottom left 23
	};

	// Index data
	GLuint indices[] = {
		0,1,2,
		0,3,2,
		4,5,6,
		4,7,6,
		8,9,10,
		8,11,10,
		12,13,14,
		12,15,14,
		16,17,18,
		16,19,18,
		20,21,22,
		20,23,22
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

	// Generate the VAO for the mesh
	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);	// activate the VAO

	// Create VBOs for the mesh
	glGenBuffers(2, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends data to the GPU

	mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]); // Activates the buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void UCreateCylinderMesh(GLMesh& mesh)
{
	GLfloat verts[] = {
		// cylinder bottom		// normals			// texture coords
		1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	0.5f,1.0f,
		.98f, 0.0f, -0.17f,		0.0f, -1.0f, 0.0f,	0.41f, 0.983f,
		.94f, 0.0f, -0.34f,		0.0f, -1.0f, 0.0f,	0.33f, 0.96f,
		.87f, 0.0f, -0.5f,		0.0f, -1.0f, 0.0f,	0.25f, 0.92f,
		.77f, 0.0f, -0.64f,		0.0f, -1.0f, 0.0f,	0.17f, 0.87f,
		.64f, 0.0f, -0.77f,		0.0f, -1.0f, 0.0f,	0.13f, 0.83f,
		.5f, 0.0f, -0.87f,		0.0f, -1.0f, 0.0f,	0.08f, 0.77f,
		.34f, 0.0f, -0.94f,		0.0f, -1.0f, 0.0f,	0.04f, 0.68f,
		.17f, 0.0f, -0.98f,		0.0f, -1.0f, 0.0f,	0.017f, 0.6f,
		0.0f, 0.0f, -1.0f,		0.0f, -1.0f, 0.0f,	0.0f,0.5f,
		-.17f, 0.0f, -0.98f,	0.0f, -1.0f, 0.0f,	0.017f, 0.41f,
		-.34f, 0.0f, -0.94f,	0.0f, -1.0f, 0.0f,	0.04f, 0.33f,
		-.5f, 0.0f, -0.87f,		0.0f, -1.0f, 0.0f,	0.08f, 0.25f,
		-.64f, 0.0f, -0.77f,	0.0f, -1.0f, 0.0f,	0.13f, 0.17f,
		-.77f, 0.0f, -0.64f,	0.0f, -1.0f, 0.0f,	0.17f, 0.13f,
		-.87f, 0.0f, -0.5f,		0.0f, -1.0f, 0.0f,	0.25f, 0.08f,
		-.94f, 0.0f, -0.34f,	0.0f, -1.0f, 0.0f,	0.33f, 0.04f,
		-.98f, 0.0f, -0.17f,	0.0f, -1.0f, 0.0f,	0.41f, 0.017f,
		-1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	0.5f, 0.0f,
		-.98f, 0.0f, 0.17f,		0.0f, -1.0f, 0.0f,	0.6f, 0.017f,
		-.94f, 0.0f, 0.34f,		0.0f, -1.0f, 0.0f,	0.68f, 0.04f,
		-.87f, 0.0f, 0.5f,		0.0f, -1.0f, 0.0f,	0.77f, 0.08f,
		-.77f, 0.0f, 0.64f,		0.0f, -1.0f, 0.0f,	0.83f, 0.13f,
		-.64f, 0.0f, 0.77f,		0.0f, -1.0f, 0.0f,	0.87f, 0.17f,
		-.5f, 0.0f, 0.87f,		0.0f, -1.0f, 0.0f,	0.92f, 0.25f,
		-.34f, 0.0f, 0.94f,		0.0f, -1.0f, 0.0f,	0.96f, 0.33f,
		-.17f, 0.0f, 0.98f,		0.0f, -1.0f, 0.0f,	0.983f, 0.41f,
		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,	1.0f, 0.5f,
		.17f, 0.0f, 0.98f,		0.0f, -1.0f, 0.0f,	0.983f, 0.6f,
		.34f, 0.0f, 0.94f,		0.0f, -1.0f, 0.0f,	0.96f, 0.68f,
		.5f, 0.0f, 0.87f,		0.0f, -1.0f, 0.0f,	0.92f, 0.77f,
		.64f, 0.0f, 0.77f,		0.0f, -1.0f, 0.0f,	0.87f, 0.83f,
		.77f, 0.0f, 0.64f,		0.0f, -1.0f, 0.0f,	0.83f, 0.87f,
		.87f, 0.0f, 0.5f,		0.0f, -1.0f, 0.0f,	0.77f, 0.92f,
		.94f, 0.0f, 0.34f,		0.0f, -1.0f, 0.0f,	0.68f, 0.96f,
		.98f, 0.0f, 0.17f,		0.0f, -1.0f, 0.0f,	0.6f, 0.983f,

		// cylinder top			// normals			// texture coords
		1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0.5f,1.0f,
		.98f, 1.0f, -0.17f,		0.0f, 1.0f, 0.0f,	0.41f, 0.983f,
		.94f, 1.0f, -0.34f,		0.0f, 1.0f, 0.0f,	0.33f, 0.96f,
		.87f, 1.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.25f, 0.92f,
		.77f, 1.0f, -0.64f,		0.0f, 1.0f, 0.0f,	0.17f, 0.87f,
		.64f, 1.0f, -0.77f,		0.0f, 1.0f, 0.0f,	0.13f, 0.83f,
		.5f, 1.0f, -0.87f,		0.0f, 1.0f, 0.0f,	0.08f, 0.77f,
		.34f, 1.0f, -0.94f,		0.0f, 1.0f, 0.0f,	0.04f, 0.68f,
		.17f, 1.0f, -0.98f,		0.0f, 1.0f, 0.0f,	0.017f, 0.6f,
		0.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f,0.5f,
		-.17f, 1.0f, -0.98f,	0.0f, 1.0f, 0.0f,	0.017f, 0.41f,
		-.34f, 1.0f, -0.94f,	0.0f, 1.0f, 0.0f,	0.04f, 0.33f,
		-.5f, 1.0f, -0.87f,		0.0f, 1.0f, 0.0f,	0.08f, 0.25f,
		-.64f, 1.0f, -0.77f,	0.0f, 1.0f, 0.0f,	0.13f, 0.17f,
		-.77f, 1.0f, -0.64f,	0.0f, 1.0f, 0.0f,	0.17f, 0.13f,
		-.87f, 1.0f, -0.5f,		0.0f, 1.0f, 0.0f,	0.25f, 0.08f,
		-.94f, 1.0f, -0.34f,	0.0f, 1.0f, 0.0f,	0.33f, 0.04f,
		-.98f, 1.0f, -0.17f,	0.0f, 1.0f, 0.0f,	0.41f, 0.017f,
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0.5f, 0.0f,
		-.98f, 1.0f, 0.17f,		0.0f, 1.0f, 0.0f,	0.6f, 0.017f,
		-.94f, 1.0f, 0.34f,		0.0f, 1.0f, 0.0f,	0.68f, 0.04f,
		-.87f, 1.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.77f, 0.08f,
		-.77f, 1.0f, 0.64f,		0.0f, 1.0f, 0.0f,	0.83f, 0.13f,
		-.64f, 1.0f, 0.77f,		0.0f, 1.0f, 0.0f,	0.87f, 0.17f,
		-.5f, 1.0f, 0.87f,		0.0f, 1.0f, 0.0f,	0.92f, 0.25f,
		-.34f, 1.0f, 0.94f,		0.0f, 1.0f, 0.0f,	0.96f, 0.33f,
		-.17f, 1.0f, 0.98f,		0.0f, 1.0f, 0.0f,	0.983f, 0.41f,
		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.5f,
		.17f, 1.0f, 0.98f,		0.0f, 1.0f, 0.0f,	0.983f, 0.6f,
		.34f, 1.0f, 0.94f,		0.0f, 1.0f, 0.0f,	0.96f, 0.68f,
		.5f, 1.0f, 0.87f,		0.0f, 1.0f, 0.0f,	0.92f, 0.77f,
		.64f, 1.0f, 0.77f,		0.0f, 1.0f, 0.0f,	0.87f, 0.83f,
		.77f, 1.0f, 0.64f,		0.0f, 1.0f, 0.0f,	0.83f, 0.87f,
		.87f, 1.0f, 0.5f,		0.0f, 1.0f, 0.0f,	0.77f, 0.92f,
		.94f, 1.0f, 0.34f,		0.0f, 1.0f, 0.0f,	0.68f, 0.96f,
		.98f, 1.0f, 0.17f,		0.0f, 1.0f, 0.0f,	0.6f, 0.983f,

		// cylinder body		// normals				// texture coords
		1.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0,1.0,
		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0,0.0,
		.98f, 0.0f, -0.17f,		1.0f, 0.0f, 0.0f,		0.0277,0.0,
		1.0f, 1.0f, 0.0f,		0.92f, 0.0f, -0.08f,	0.0,1.0,
		.98f, 1.0f, -0.17f,		0.92f, 0.0f, -0.08f,	0.0277,1.0,
		.98f, 0.0f, -0.17f,		0.92f, 0.0f, -0.08f,	0.0277,0.0,
		.94f, 0.0f, -0.34f,		0.83f, 0.0f, -0.17f,	0.0554,0.0,
		.98f, 1.0f, -0.17f,		0.83f, 0.0f, -0.17f,	0.0277,1.0,
		.94f, 1.0f, -0.34f,		0.83f, 0.0f, -0.17f,	0.0554,1.0,
		.94f, 0.0f, -0.34f,		0.75f, 0.0f, -0.25f,	0.0554,0.0,
		.87f, 0.0f, -0.5f,		0.75f, 0.0f, -0.25f,	0.0831,0.0,
		.94f, 1.0f, -0.34f,		0.75f, 0.0f, -0.25f,	0.0554,1.0,
		.87f, 1.0f, -0.5f,		0.67f, 0.0f, -0.33f,	0.0831,1.0,
		.87f, 0.0f, -0.5f,		0.67f, 0.0f, -0.33f,	0.0831,0.0,
		.77f, 0.0f, -0.64f,		0.67f, 0.0f, -0.33f,	0.1108,0.0,
		.87f, 1.0f, -0.5f,		0.58f, 0.0f, -0.42f,	0.0831,1.0,
		.77f, 1.0f, -0.64f,		0.58f, 0.0f, -0.42f,	0.1108,1.0,
		.77f, 0.0f, -0.64f,		0.58f, 0.0f, -0.42f,	0.1108,0.0,
		.64f, 0.0f, -0.77f,		0.5f, 0.0f, -0.5f,		0.1385,0.0,
		.77f, 1.0f, -0.64f,		0.5f, 0.0f, -0.5f,		0.1108,1.0,
		.64f, 1.0f, -0.77f,		0.5f, 0.0f, -0.5f,		0.1385,1.0,
		.64f, 0.0f, -0.77f,		0.42f, 0.0f, -0.58f,	0.1385,0.0,
		.5f, 0.0f, -0.87f,		0.42f, 0.0f, -0.58f,	0.1662,0.0,
		.64f, 1.0f, -0.77f,		0.42f, 0.0f, -0.58f,	0.1385, 1.0,
		.5f, 1.0f, -0.87f,		0.33f, 0.0f, -0.67f,	0.1662, 1.0,
		.5f, 0.0f, -0.87f,		0.33f, 0.0f, -0.67f,	0.1662, 0.0,
		.34f, 0.0f, -0.94f,		0.33f, 0.0f, -0.67f,	0.1939, 0.0,
		.5f, 1.0f, -0.87f,		0.25f, 0.0f, -0.75f,	0.1662, 1.0,
		.34f, 1.0f, -0.94f,		0.25f, 0.0f, -0.75f,	0.1939, 1.0,
		.34f, 0.0f, -0.94f,		0.25f, 0.0f, -0.75f,	0.1939, 0.0,
		.17f, 0.0f, -0.98f,		0.17f, 0.0f, -0.83f,	0.2216, 0.0,
		.34f, 1.0f, -0.94f,		0.17f, 0.0f, -0.83f,	0.1939, 1.0,
		.17f, 1.0f, -0.98f,		0.17f, 0.0f, -0.83f,	0.2216, 1.0,
		.17f, 0.0f, -0.98f,		0.08f, 0.0f, -0.92f,	0.2216, 0.0,
		0.0f, 0.0f, -1.0f,		0.08f, 0.0f, -0.92f,	0.2493, 0.0,
		.17f, 1.0f, -0.98f,		0.08f, 0.0f, -0.92f,	0.2216, 1.0,
		0.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.2493, 1.0,
		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.2493, 0.0,
		-.17f, 0.0f, -0.98f,	0.0f, 0.0f, -1.0f,		0.277, 0.0,
		0.0f, 1.0f, -1.0f,		0.08f, 0.0f, -1.08f,	0.2493, 1.0,
		-.17f, 1.0f, -0.98f,	-0.08f, 0.0f, -0.92f,	0.277, 1.0,
		-.17f, 0.0f, -0.98f,	-0.08f, 0.0f, -0.92f,	0.277, 0.0,
		-.34f, 0.0f, -0.94f,	-0.08f, 0.0f, -0.92f,	0.3047, 0.0,
		-.17f, 1.0f, -0.98f,	-0.08f, 0.0f, -0.92f,	0.277, 1.0,
		-.34f, 1.0f, -0.94f,	-0.17f, 0.0f, -0.83f,	0.3047, 1.0,
		-.34f, 0.0f, -0.94f,	-0.17f, 0.0f, -0.83f,	0.3047, 0.0,
		-.5f, 0.0f, -0.87f,		-0.17f, 0.0f, -0.83f,	0.3324, 0.0,
		-.34f, 1.0f, -0.94f,	-0.25f, 0.0f, -0.75f,	0.3047, 1.0,
		-.5f, 1.0f, -0.87f,		-0.25f, 0.0f, -0.75f,	0.3324, 1.0,
		-.5f, 0.0f, -0.87f,		-0.25f, 0.0f, -0.75f,	0.3324, 0.0,
		-.64f, 0.0f, -0.77f,	-0.33f, 0.0f, -0.67f,	0.3601, 0.0,
		-.5f, 1.0f, -0.87f,		-0.33f, 0.0f, -0.67f,	0.3324, 1.0,
		-.64f, 1.0f, -0.77f,	-0.33f, 0.0f, -0.67f,	0.3601, 1.0,
		-.64f, 0.0f, -0.77f,	-0.42f, 0.0f, -0.58f,	0.3601, 0.0,
		-.77f, 0.0f, -0.64f,	-0.42f, 0.0f, -0.58f,	0.3878, 0.0,
		-.64f, 1.0f, -0.77f,	-0.42f, 0.0f, -0.58f,	0.3601, 1.0,
		-.77f, 1.0f, -0.64f,	-0.5f, 0.0f, -0.5f,		0.3878, 1.0,
		-.77f, 0.0f, -0.64f,	-0.5f, 0.0f, -0.5f,		0.3878, 0.0,
		-.87f, 0.0f, -0.5f,		-0.5f, 0.0f, -0.5f,		0.4155, 0.0,
		-.77f, 1.0f, -0.64f,	-0.58f, 0.0f, -0.42f,	0.3878, 1.0,
		-.87f, 1.0f, -0.5f,		-0.58f, 0.0f, -0.42f,	0.4155, 1.0,
		-.87f, 0.0f, -0.5f,		-0.58f, 0.0f, -0.42f,	0.4155, 0.0,
		-.94f, 0.0f, -0.34f,	-0.67f, 0.0f, -0.33f,	0.4432, 0.0,
		-.87f, 1.0f, -0.5f,		-0.67f, 0.0f, -0.33f,	0.4155, 1.0,
		-.94f, 1.0f, -0.34f,	-0.67f, 0.0f, -0.33f,	0.4432, 1.0,
		-.94f, 0.0f, -0.34f,	-0.75f, 0.0f, -0.25f,	0.4432, 0.0,
		-.98f, 0.0f, -0.17f,	-0.75f, 0.0f, -0.25f,	0.4709, 0.0,
		-.94f, 1.0f, -0.34f,	-0.75f, 0.0f, -0.25f,	0.4432, 1.0,
		-.98f, 1.0f, -0.17f,	-0.83f, 0.0f, -0.17f,	0.4709, 1.0,
		-.98f, 0.0f, -0.17f,	-0.83f, 0.0f, -0.17f,	0.4709, 0.0,
		-1.0f, 0.0f, 0.0f,		-0.83f, 0.0f, -0.17f,	0.4986, 0.0,
		-.98f, 1.0f, -0.17f,	-0.92f, 0.0f, -0.08f,	0.4709, 1.0,
		-1.0f, 1.0f, 0.0f,		-0.92f, 0.0f, -0.08f,	0.4986, 1.0,
		-1.0f, 0.0f, 0.0f,		-0.92f, 0.0f, -0.08f,	0.4986, 0.0,
		-.98f, 0.0f, 0.17f,		-1.0f, 0.0f, 0.0f,		0.5263, 0.0,
		-1.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.4986, 1.0,
		-.98f, 1.0f, 0.17f,		-1.0f, 0.0f, 0.0f,		0.5263, 1.0,
		-.98f, 0.0f, 0.17f,		-0.92f, 0.0f, 0.08f,	0.5263, 0.0,
		-.94f, 0.0f, 0.34f,		-0.92f, 0.0f, 0.08f,	0.554, 0.0,
		-.98f, 1.0f, 0.17f,		-0.92f, 0.0f, 0.08f,	0.5263, 1.0,
		-.94f, 1.0f, 0.34f,		-0.83f, 0.0f, 0.17f,	0.554, 1.0,
		-.94f, 0.0f, 0.34f,		-0.83f, 0.0f, 0.17f,	0.554, 0.0,
		-.87f, 0.0f, 0.5f,		-0.83f, 0.0f, 0.17f,	0.5817, 0.0,
		-.94f, 1.0f, 0.34f,		-0.75f, 0.0f, 0.25f,	0.554, 1.0,
		-.87f, 1.0f, 0.5f,		-0.75f, 0.0f, 0.25f,	0.5817, 1.0,
		-.87f, 0.0f, 0.5f,		-0.75f, 0.0f, 0.25f,	0.5817, 0.0,
		-.77f, 0.0f, 0.64f,		-0.67f, 0.0f, 0.33f,	0.6094, 0.0,
		-.87f, 1.0f, 0.5f,		-0.67f, 0.0f, 0.33f,	0.5817, 1.0,
		-.77f, 1.0f, 0.64f,		-0.67f, 0.0f, 0.33f,	0.6094, 1.0,
		-.77f, 0.0f, 0.64f,		-0.58f, 0.0f, 0.42f,	0.6094, 0.0,
		-.64f, 0.0f, 0.77f,		-0.58f, 0.0f, 0.42f,	0.6371, 0.0,
		-.77f, 1.0f, 0.64f,		-0.58f, 0.0f, 0.42f,	0.6094, 1.0,
		-.64f, 1.0f, 0.77f,		-0.5f, 0.0f, 0.5f,		0.6371, 1.0,
		-.64f, 0.0f, 0.77f,		-0.5f, 0.0f, 0.5f,		0.6371, 0.0,
		-.5f, 0.0f, 0.87f,		-0.5f, 0.0f, 0.5f,		0.6648, 0.0,
		-.64f, 1.0f, 0.77f,		-0.42f, 0.0f, 0.58f,	0.6371, 1.0,
		-.5f, 1.0f, 0.87f,		-0.42f, 0.0f, 0.58f,	0.6648, 1.0,
		-.5f, 0.0f, 0.87f,		-0.42f, 0.0f, 0.58f,	0.6648, 0.0,
		-.34f, 0.0f, 0.94f,		-0.33f, 0.0f, 0.67f,	0.6925, 0.0,
		-.5f, 1.0f, 0.87f,		-0.33f, 0.0f, 0.67f,	0.6648, 1.0,
		-.34f, 1.0f, 0.94f,		-0.33f, 0.0f, 0.67f,	0.6925, 1.0,
		-.34f, 0.0f, 0.94f,		-0.25f, 0.0f, 0.75f,	0.6925, 0.0,
		-.17f, 0.0f, 0.98f,		-0.25f, 0.0f, 0.75f,	0.7202, 0.0,
		-.34f, 1.0f, 0.94f,		-0.25f, 0.0f, 0.75f,	0.6925, 1.0,
		-.17f, 1.0f, 0.98f,		-0.17f, 0.0f, 0.83f,	0.7202, 1.0,
		-.17f, 0.0f, 0.98f,		-0.17f, 0.0f, 0.83f,	0.7202, 0.0,
		0.0f, 0.0f, 1.0f,		-0.17f, 0.0f, 0.83f,	0.7479, 0.0,
		-.17f, 1.0f, 0.98f,		-0.08f, 0.0f, 0.92f,	0.7202, 1.0,
		0.0f, 1.0f, 1.0f,		-0.08f, 0.0f, 0.92f,	0.7479, 1.0,
		0.0f, 0.0f, 1.0f,		-0.08f, 0.0f, 0.92f,	0.7479, 0.0,
		.17f, 0.0f, 0.98f,		-0.0f, 0.0f, 1.0f,		0.7756, 0.0,
		0.0f, 1.0f, 1.0f,		-0.0f, 0.0f, 1.0f,		0.7479, 1.0,
		.17f, 1.0f, 0.98f,		-0.0f, 0.0f, 1.0f,		0.7756, 1.0,
		.17f, 0.0f, 0.98f,		0.08f, 0.0f, 0.92f,		0.7756, 0.0,
		.34f, 0.0f, 0.94f,		0.08f, 0.0f, 0.92f,		0.8033, 0.0,
		.17f, 1.0f, 0.98f,		0.08f, 0.0f, 0.92f,		0.7756, 1.0,
		.34f, 1.0f, 0.94f,		0.17f, 0.0f, 0.83f,		0.8033, 1.0,
		.34f, 0.0f, 0.94f,		0.17f, 0.0f, 0.83f,		0.8033, 0.0,
		.5f, 0.0f, 0.87f,		0.17f, 0.0f, 0.83f,		0.831, 0.0,
		.34f, 1.0f, 0.94f,		0.25f, 0.0f, 0.75f,		0.8033, 1.0,
		.5f, 1.0f, 0.87f,		0.25f, 0.0f, 0.75f,		0.831, 1.0,
		.5f, 0.0f, 0.87f,		0.25f, 0.0f, 0.75f,		0.831, 0.0,
		.64f, 0.0f, 0.77f,		0.33f, 0.0f, 0.67f,		0.8587, 0.0,
		.5f, 1.0f, 0.87f,		0.33f, 0.0f, 0.67f,		0.831, 1.0,
		.64f, 1.0f, 0.77f,		0.33f, 0.0f, 0.67f,		0.8587, 1.0,
		.64f, 0.0f, 0.77f,		0.42f, 0.0f, 0.58f,		0.8587, 0.0,
		.77f, 0.0f, 0.64f,		0.42f, 0.0f, 0.58f,		0.8864, 0.0,
		.64f, 1.0f, 0.77f,		0.42f, 0.0f, 0.58f,		0.8587, 1.0,
		.77f, 1.0f, 0.64f,		0.5f, 0.0f, 0.5f,		0.8864, 1.0,
		.77f, 0.0f, 0.64f,		0.5f, 0.0f, 0.5f,		0.8864, 0.0,
		.87f, 0.0f, 0.5f,		0.5f, 0.0f, 0.5f,		0.9141, 0.0,
		.77f, 1.0f, 0.64f,		0.58f, 0.0f, 0.42f,		0.8864, 1.0,
		.87f, 1.0f, 0.5f,		0.58f, 0.0f, 0.42f,		0.9141, 1.0,
		.87f, 0.0f, 0.5f,		0.58f, 0.0f, 0.42f,		0.9141, 0.0,
		.94f, 0.0f, 0.34f,		0.67f, 0.0f, 0.33f,		0.9418, 0.0,
		.87f, 1.0f, 0.5f,		0.67f, 0.0f, 0.33f,		0.9141, 1.0,
		.94f, 1.0f, 0.34f,		0.67f, 0.0f, 0.33f,		0.9418, 1.0,
		.94f, 0.0f, 0.34f,		0.75f, 0.0f, 0.25f,		0.9418, 0.0,
		.98f, 0.0f, 0.17f,		0.75f, 0.0f, 0.25f,		0.9695, 0.0,
		.94f, 1.0f, 0.34f,		0.75f, 0.0f, 0.25f,		0.9418, 0.0,
		.98f, 1.0f, 0.17f,		0.83f, 0.0f, 0.17f,		0.9695, 1.0,
		.98f, 0.0f, 0.17f,		0.83f, 0.0f, 0.17f,		0.9695, 0.0,
		1.0f, 0.0f, 0.0f,		0.83f, 0.0f, 0.17f,		1.0, 0.0,
		.98f, 1.0f, 0.17f,		0.92f, 0.0f, 0.08f,		0.9695, 1.0,
		1.0f, 1.0f, 0.0f,		0.92f, 0.0f, 0.08f,		1.0, 1.0,
		1.0f, 0.0f, 0.0f,		0.92f, 0.0f, 0.08f,		1.0, 0.0
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nIndices = 0;

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create VBO
	glGenBuffers(1, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void UCreateConeMesh(GLMesh& mesh)
{
	GLfloat verts[] = {
		// cone bottom			// normals			// texture coords
		1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	0.5f,1.0f,
		.98f, 0.0f, -0.17f,		0.0f, -1.0f, 0.0f,	0.41f, 0.983f,
		.94f, 0.0f, -0.34f,		0.0f, -1.0f, 0.0f,	0.33f, 0.96f,
		.87f, 0.0f, -0.5f,		0.0f, -1.0f, 0.0f,	0.25f, 0.92f,
		.77f, 0.0f, -0.64f,		0.0f, -1.0f, 0.0f,	0.17f, 0.87f,
		.64f, 0.0f, -0.77f,		0.0f, -1.0f, 0.0f,	0.13f, 0.83f,
		.5f, 0.0f, -0.87f,		0.0f, -1.0f, 0.0f,	0.08f, 0.77f,
		.34f, 0.0f, -0.94f,		0.0f, -1.0f, 0.0f,	0.04f, 0.68f,
		.17f, 0.0f, -0.98f,		0.0f, -1.0f, 0.0f,	0.017f, 0.6f,
		0.0f, 0.0f, -1.0f,		0.0f, -1.0f, 0.0f,	0.0f,0.5f,
		-.17f, 0.0f, -0.98f,	0.0f, -1.0f, 0.0f,	0.017f, 0.41f,
		-.34f, 0.0f, -0.94f,	0.0f, -1.0f, 0.0f,	0.04f, 0.33f,
		-.5f, 0.0f, -0.87f,		0.0f, -1.0f, 0.0f,	0.08f, 0.25f,
		-.64f, 0.0f, -0.77f,	0.0f, -1.0f, 0.0f,	0.13f, 0.17f,
		-.77f, 0.0f, -0.64f,	0.0f, -1.0f, 0.0f,	0.17f, 0.13f,
		-.87f, 0.0f, -0.5f,		0.0f, -1.0f, 0.0f,	0.25f, 0.08f,
		-.94f, 0.0f, -0.34f,	0.0f, -1.0f, 0.0f,	0.33f, 0.04f,
		-.98f, 0.0f, -0.17f,	0.0f, -1.0f, 0.0f,	0.41f, 0.017f,
		-1.0f, 0.0f, 0.0f,		0.0f, -1.0f, 0.0f,	0.5f, 0.0f,
		-.98f, 0.0f, 0.17f,		0.0f, -1.0f, 0.0f,	0.6f, 0.017f,
		-.94f, 0.0f, 0.34f,		0.0f, -1.0f, 0.0f,	0.68f, 0.04f,
		-.87f, 0.0f, 0.5f,		0.0f, -1.0f, 0.0f,	0.77f, 0.08f,
		-.77f, 0.0f, 0.64f,		0.0f, -1.0f, 0.0f,	0.83f, 0.13f,
		-.64f, 0.0f, 0.77f,		0.0f, -1.0f, 0.0f,	0.87f, 0.17f,
		-.5f, 0.0f, 0.87f,		0.0f, -1.0f, 0.0f,	0.92f, 0.25f,
		-.34f, 0.0f, 0.94f,		0.0f, -1.0f, 0.0f,	0.96f, 0.33f,
		-.17f, 0.0f, 0.98f,		0.0f, -1.0f, 0.0f,	0.983f, 0.41f,
		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,	1.0f, 0.5f,
		.17f, 0.0f, 0.98f,		0.0f, -1.0f, 0.0f,	0.983f, 0.6f,
		.34f, 0.0f, 0.94f,		0.0f, -1.0f, 0.0f,	0.96f, 0.68f,
		.5f, 0.0f, 0.87f,		0.0f, -1.0f, 0.0f,	0.92f, 0.77f,
		.64f, 0.0f, 0.77f,		0.0f, -1.0f, 0.0f,	0.87f, 0.83f,
		.77f, 0.0f, 0.64f,		0.0f, -1.0f, 0.0f,	0.83f, 0.87f,
		.87f, 0.0f, 0.5f,		0.0f, -1.0f, 0.0f,	0.77f, 0.92f,
		.94f, 0.0f, 0.34f,		0.0f, -1.0f, 0.0f,	0.68f, 0.96f,
		.98f, 0.0f, 0.17f,		0.0f, -1.0f, 0.0f,	0.6f, 0.983f,

		// cone sides		// normals									// texture coords
		1.0f, 0.0f, 0.0f,		-0.993150651f, 0.0f, 0.116841137f, 		0.0f,0.0f,
		0.0f, 1.0f, 0.0f,		-0.993150651f, 0.0f, 0.116841137f, 		0.5f, 1.0f,
		.98f, 0.0f, -0.17f,		-0.993150651f, 0.0f, 0.116841137f, 		0.0277,0.0,
		.98f, 0.0f, -0.17f,		-0.973417103f, 0.0f, 0.229039446f, 		0.0277,0.0,
		0.0f, 1.0f, 0.0f,		-0.973417103f, 0.0f, 0.229039446f, 		0.5f, 1.0f,
		.94f, 0.0f, -0.34f,		-0.973417103f, 0.0f, 0.229039446f, 		0.0554,0.0f,
		.94f, 0.0f, -0.34f,		-0.916157305f, 0.0f, 0.400818795f, 		0.0554,0.0f,
		0.0f, 1.0f, 0.0f,		-0.916157305f, 0.0f, 0.400818795f, 		0.5f, 1.0f,
		.87f, 0.0f, -0.5f,		-0.916157305f, 0.0f, 0.400818795f, 		0.0831,0.0f,
		.87f, 0.0f, -0.5f,		-0.813733339f, 0.0f, 0.581238329f, 		0.0831,0.0f,
		0.0f, 1.0f, 0.0f,		-0.813733339f, 0.0f, 0.581238329f, 		0.5f, 1.0f,
		.77f, 0.0f, -0.64f,		-0.813733339f, 0.0f, 0.581238329f, 		0.1108f, 0.0f,
		.77f, 0.0f, -0.64f,		-0.707106769f, 0.0f, 0.707106769f, 		0.1108f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.707106769f, 0.0f, 0.707106769f, 		0.5f, 1.0f,
		.64f, 0.0f, -0.77f,		-0.707106769f, 0.0f, 0.707106769f, 		0.1385f, 0.0f,
		.64f, 0.0f, -0.77f,		-0.581238329f, 0.0f, 0.813733339f, 		0.1385f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.581238329f, 0.0f, 0.813733339f, 		0.5f, 1.0f,
		.5f, 0.0f, -0.87f,		-0.581238329f, 0.0f, 0.813733339f, 		0.1662f, 0.0f,
		.5f, 0.0f, -0.87f,		-0.400818795f, 0.0f, 0.916157305f, 		0.1662f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.400818795f, 0.0f, 0.916157305f, 		0.5f, 1.0f,
		.34f, 0.0f, -0.94f,		-0.400818795f, 0.0f, 0.916157305f, 		0.1939f, 0.0f,
		.34f, 0.0f, -0.94f,		-0.229039446f, 0.0f, 0.973417103f, 		0.1939f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.229039446f, 0.0f, 0.973417103f, 		0.5f, 1.0f,
		.17f, 0.0f, -0.98f,		-0.229039446f, 0.0f, 0.973417103f, 		0.2216f, 0.0f,
		.17f, 0.0f, -0.98f,		-0.116841137f, 0.0f, 0.993150651f, 		0.2216f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.116841137f, 0.0f, 0.993150651f, 		0.5f, 1.0f,
		0.0f, 0.0f, -1.0f,		-0.116841137f, 0.0f, 0.993150651f, 		0.2493f, 0.0f,
		0.0f, 0.0f, -1.0f,		0.116841137f, 0.0f, 0.993150651f, 		0.2493f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.116841137f, 0.0f, 0.993150651f, 		0.5f, 1.0f,
		-.17f, 0.0f, -0.98f,	0.116841137f, 0.0f, 0.993150651f, 		0.277f, 0.0f,
		-.17f, 0.0f, -0.98f,	0.229039446f, 0.0f, 0.973417103f, 		0.277f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.229039446f, 0.0f, 0.973417103f, 		0.5f, 1.0f,
		-.34f, 0.0f, -0.94f,	0.229039446f, 0.0f, 0.973417103f, 		0.3047f, 0.0f,
		-.34f, 0.0f, -0.94f,	0.400818795f, 0.0f, 0.916157305f, 		0.3047f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.400818795f, 0.0f, 0.916157305f, 		0.5f, 1.0f,
		-.5f, 0.0f, -0.87f,		0.400818795f, 0.0f, 0.916157305f, 		0.3324f, 0.0f,
		-.5f, 0.0f, -0.87f,		0.581238329f, 0.0f, 0.813733339f, 		0.3324f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.581238329f, 0.0f, 0.813733339f, 		0.5f, 1.0f,
		-.64f, 0.0f, -0.77f,	0.581238329f, 0.0f, 0.813733339f, 		0.3601f, 0.0f,
		-.64f, 0.0f, -0.77f,	0.707106769f, 0.0f, 0.707106769f, 		0.3601f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.707106769f, 0.0f, 0.707106769f, 		0.5f, 1.0f,
		-.77f, 0.0f, -0.64f,	0.707106769f, 0.0f, 0.707106769f, 		0.3878f, 0.0f,
		-.77f, 0.0f, -0.64f,	0.813733339f, 0.0f, 0.581238329f, 		0.3878f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.813733339f, 0.0f, 0.581238329f, 		0.5f, 1.0f,
		-.87f, 0.0f, -0.5f,		0.813733339f, 0.0f, 0.581238329f, 		0.4155f, 0.0f,
		-.87f, 0.0f, -0.5f,		0.916157305f, 0.0f, 0.400818795f, 		0.4155f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.916157305f, 0.0f, 0.400818795f, 		0.5f, 1.0f,
		-.94f, 0.0f, -0.34f,	0.916157305f, 0.0f, 0.400818795f, 		0.4432f, 0.0f,
		-.94f, 0.0f, -0.34f,	0.973417103f, 0.0f, 0.229039446f, 		0.4432f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.973417103f, 0.0f, 0.229039446f, 		0.5f, 1.0f,
		-.98f, 0.0f, -0.17f,	0.973417103f, 0.0f, 0.229039446f, 		0.4709f, 0.0f,
		-.98f, 0.0f, -0.17f,	0.993150651f, 0.0f, 0.116841137f, 		0.4709f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.993150651f, 0.0f, 0.116841137f, 		0.5f, 1.0f,
		-1.0f, 0.0f, 0.0f,		0.993150651f, 0.0f, 0.116841137f, 		0.4986f, 0.0f,
		-1.0f, 0.0f, 0.0f,		0.993150651f, 0.0f, -0.116841137f, 		0.4986f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.993150651f, 0.0f, -0.116841137f, 		0.5f, 1.0f,
		-.98f, 0.0f, 0.17f,		0.993150651f, 0.0f, -0.116841137f, 		0.5263f, 0.0f,
		-.98f, 0.0f, 0.17f,		0.973417103f, 0.0f, -0.229039446f, 		0.5263f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.973417103f, 0.0f, -0.229039446f, 		0.5f, 1.0f,
		-.94f, 0.0f, 0.34f,		0.973417103f, 0.0f, -0.229039446f, 		0.554f, 0.0f,
		-.94f, 0.0f, 0.34f,		0.916157305f, 0.0f, -0.400818795f, 		0.554f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.916157305f, 0.0f, -0.400818795f, 		0.5f, 1.0f,
		-.87f, 0.0f, 0.5f,		0.916157305f, 0.0f, -0.400818795f, 		0.5817f, 0.0f,
		-.87f, 0.0f, 0.5f,		0.813733339f, 0.0f, -0.581238329f, 		0.5817f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.813733339f, 0.0f, -0.581238329f, 		0.5f, 1.0f,
		-.77f, 0.0f, 0.64f,		0.813733339f, 0.0f, -0.581238329f, 		0.6094f, 0.0f,
		-.77f, 0.0f, 0.64f,		0.707106769f, 0.0f, -0.707106769f, 		0.6094f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.707106769f, 0.0f, -0.707106769f, 		0.5f, 1.0f,
		-.64f, 0.0f, 0.77f,		0.707106769f, 0.0f, -0.707106769f, 		0.6371f, 0.0f,
		-.64f, 0.0f, 0.77f,		0.581238329f, 0.0f, -0.813733339f, 		0.6371f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.581238329f, 0.0f, -0.813733339f, 		0.5f, 1.0f,
		-.5f, 0.0f, 0.87f,		0.581238329f, 0.0f, -0.813733339f, 		0.6648f, 0.0f,
		-.5f, 0.0f, 0.87f,		0.400818795f, 0.0f, -0.916157305f, 		0.6648f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.400818795f, 0.0f, -0.916157305f, 		0.5f, 1.0f,
		-.34f, 0.0f, 0.94f,		0.400818795f, 0.0f, -0.916157305f, 		0.6925f, 0.0f,
		-.34f, 0.0f, 0.94f,		0.229039446f, 0.0f, -0.973417103f, 		0.6925f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.229039446f, 0.0f, -0.973417103f, 		0.5f, 1.0f,
		-.17f, 0.0f, 0.98f,		0.229039446f, 0.0f, -0.973417103f, 		0.7202f, 0.0f,
		-.17f, 0.0f, 0.98f,		0.116841137f, 0.0f, -0.993150651f, 		0.7202f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.116841137f, 0.0f, -0.993150651f, 		0.5f, 1.0f,
		0.0f, 0.0f, 1.0f,		0.116841137f, 0.0f, -0.993150651f, 		0.7479f, 0.0f,
		0.0f, 0.0f, 1.0f,		-0.116841137f, 0.0f, -0.993150651f, 	0.7479f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.116841137f, 0.0f, -0.993150651f, 	0.5f, 1.0f,
		.17f, 0.0f, 0.98f,		-0.116841137f, 0.0f, -0.993150651f, 	0.7756f, 0.0f,
		.17f, 0.0f, 0.98f,		-0.229039446f, 0.0f, -0.973417103f, 	0.7756f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.229039446f, 0.0f, -0.973417103f, 	0.5f, 1.0f,
		.34f, 0.0f, 0.94f,		-0.229039446f, 0.0f, -0.973417103f, 	0.8033f, 0.0f,
		.34f, 0.0f, 0.94f,		-0.400818795f, 0.0f, -0.916157305f, 	0.8033f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.400818795f, 0.0f, -0.916157305f, 	0.5f, 1.0f,
		.5f, 0.0f, 0.87f,		-0.400818795f, 0.0f, -0.916157305f, 	0.831f, 0.0f,
		.5f, 0.0f, 0.87f,		-0.581238329f, 0.0f, -0.813733339f, 	0.831f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.581238329f, 0.0f, -0.813733339f, 	0.5f, 1.0f,
		.64f, 0.0f, 0.77f,		-0.581238329f, 0.0f, -0.813733339f, 	0.8587f, 0.0f,
		.64f, 0.0f, 0.77f,		-0.707106769f, 0.0f, -0.707106769f, 	0.8587f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.707106769f, 0.0f, -0.707106769f, 	0.5f, 1.0f,
		.77f, 0.0f, 0.64f,		-0.707106769f, 0.0f, -0.707106769f, 	0.8864f, 0.0f,
		.77f, 0.0f, 0.64f,		-0.813733339f, 0.0f, -0.581238329f, 	0.8864f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.813733339f, 0.0f, -0.581238329f, 	0.5f, 1.0f,
		.87f, 0.0f, 0.5f,		-0.813733339f, 0.0f, -0.581238329f, 	0.9141f, 0.0f,
		.87f, 0.0f, 0.5f,		-0.916157305f, 0.0f, -0.400818795f, 	0.9141f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.916157305f, 0.0f, -0.400818795f, 	0.5f, 1.0f,
		.94f, 0.0f, 0.34f,		-0.916157305f, 0.0f, -0.400818795f, 	0.9418f, 0.0f,
		.94f, 0.0f, 0.34f,		-0.973417103f, 0.0f, -0.229039446f, 	0.9418f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.973417103f, 0.0f, -0.229039446f, 	0.5f, 1.0f,
		.98f, 0.0f, 0.17f,		-0.973417103f, 0.0f, -0.229039446f, 	0.9695f, 0.0f,
		.98f, 0.0f, 0.17f,		-0.993150651f, 0.0f, -0.116841137f, 	0.9695f, 0.0f,
		0.0f, 1.0f, 0.0f,		-0.993150651f, 0.0f, -0.116841137f, 	0.5f, 1.0f,
		1.0f, 0.0f, 0.0f,		-0.993150651f, 0.0f, -0.116841137f, 	0.0f, 0.0f
	};

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));
	mesh.nIndices = 0;

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create VBO
	glGenBuffers(1, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void UCreateHalfTorusMesh(GLMesh& mesh)
{
	int _mainSegments = 30;
	int _tubeSegments = 30;
	float _mainRadius = 1.0f;
	float _tubeRadius = .1f;

	auto mainSegmentAngleStep = glm::radians(180.0f / float(_mainSegments));
	auto tubeSegmentAngleStep = glm::radians(180.0f / float(_tubeSegments));

	std::vector<glm::vec3> vertex_list;
	std::vector<std::vector<glm::vec3>> segments_list;
	std::vector<glm::vec3> normals_list;
	std::vector<glm::vec2> texture_coords;
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 normal;
	glm::vec3 vertex;
	glm::vec2 text_coord;

	// generate the torus vertices
	auto currentMainSegmentAngle = 0.0f;
	for (auto i = 0; i < _mainSegments; i++)
	{
		// Calculate sine and cosine of main segment angle
		auto sinMainSegment = sin(currentMainSegmentAngle);
		auto cosMainSegment = cos(currentMainSegmentAngle);
		auto currentTubeSegmentAngle = 0.0f;
		std::vector<glm::vec3> segment_points;
		for (auto j = 0; j < _tubeSegments; j++)
		{
			// Calculate sine and cosine of tube segment angle
			auto sinTubeSegment = sin(currentTubeSegmentAngle);
			auto cosTubeSegment = cos(currentTubeSegmentAngle);

			// Calculate vertex position on the surface of torus
			auto surfacePosition = glm::vec3(
				(_mainRadius + _tubeRadius * cosTubeSegment) * cosMainSegment,
				(_mainRadius + _tubeRadius * cosTubeSegment) * sinMainSegment,
				_tubeRadius * sinTubeSegment);

			//vertex_list.push_back(surfacePosition);
			segment_points.push_back(surfacePosition);

			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}
		segments_list.push_back(segment_points);
		segment_points.clear();

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	float horizontalStep = 1.0 / _mainSegments;
	float verticalStep = 1.0 / _tubeSegments;
	float u = 0.0;
	float v = 0.0;

	// connect the various segments together, forming triangles
	for (int i = 0; i < _mainSegments; i++)
	{
		for (int j = 0; j < _tubeSegments; j++)
		{
			if (((i + 1) < _mainSegments) && ((j + 1) < _tubeSegments))
			{
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
				vertex_list.push_back(segments_list[i][j + 1]);
				normal = normalize(segments_list[i][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v + verticalStep));
				vertex_list.push_back(segments_list[i + 1][j + 1]);
				normal = normalize(segments_list[i + 1][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v + verticalStep));
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
				vertex_list.push_back(segments_list[i + 1][j]);
				normal = normalize(segments_list[i + 1][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v));
				vertex_list.push_back(segments_list[i + 1][j + 1]);
				normal = normalize(segments_list[i + 1][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v - verticalStep));
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
			}
			else
			{
				if (((i + 1) == _mainSegments) && ((j + 1) == _tubeSegments))
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][0]);
					normal = normalize(segments_list[i][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, 0));
					vertex_list.push_back(segments_list[0][0]);
					normal = normalize(segments_list[0][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[0][j]);
					normal = normalize(segments_list[0][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v));
					vertex_list.push_back(segments_list[0][0]);
					normal = normalize(segments_list[0][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}
				else if ((i + 1) == _mainSegments)
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][j + 1]);
					normal = normalize(segments_list[i][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v + verticalStep));
					vertex_list.push_back(segments_list[0][j + 1]);
					normal = normalize(segments_list[0][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v + verticalStep));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[0][j]);
					normal = normalize(segments_list[0][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v));
					vertex_list.push_back(segments_list[0][j + 1]);
					normal = normalize(segments_list[0][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v + verticalStep));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}
				else if ((j + 1) == _tubeSegments)
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][0]);
					normal = normalize(segments_list[i][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, 0));
					vertex_list.push_back(segments_list[i + 1][0]);
					normal = normalize(segments_list[i + 1][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i + 1][j]);
					normal = normalize(segments_list[i + 1][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, v));
					vertex_list.push_back(segments_list[i + 1][0]);
					normal = normalize(segments_list[i + 1][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}

			}
			v += verticalStep;
		}
		v = 0.0;
		u += horizontalStep;
	}

	std::vector<GLfloat> combined_values;

	// combine interleaved vertices, normals, and texture coords
	for (int i = 0; i < vertex_list.size(); i++)
	{
		vertex = vertex_list[i];
		normal = normals_list[i];
		text_coord = texture_coords[i];
		combined_values.push_back(vertex.x);
		combined_values.push_back(vertex.y);
		combined_values.push_back(vertex.z);
		combined_values.push_back(normal.x);
		combined_values.push_back(normal.y);
		combined_values.push_back(normal.z);
		combined_values.push_back(text_coord.x);
		combined_values.push_back(text_coord.y);
	}

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = vertex_list.size();
	mesh.nIndices = 0;

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create VBOs
	glGenBuffers(1, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * combined_values.size(), combined_values.data(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}

void UCreateTorusMesh(GLMesh& mesh)
{
	int _mainSegments = 30;
	int _tubeSegments = 30;
	float _mainRadius = 1.0f;
	float _tubeRadius = .1f;

	auto mainSegmentAngleStep = glm::radians(360.0f / float(_mainSegments));
	auto tubeSegmentAngleStep = glm::radians(360.0f / float(_tubeSegments));

	std::vector<glm::vec3> vertex_list;
	std::vector<std::vector<glm::vec3>> segments_list;
	std::vector<glm::vec3> normals_list;
	std::vector<glm::vec2> texture_coords;
	glm::vec3 center(0.0f, 0.0f, 0.0f);
	glm::vec3 normal;
	glm::vec3 vertex;
	glm::vec2 text_coord;

	// generate the torus vertices
	auto currentMainSegmentAngle = 0.0f;
	for (auto i = 0; i < _mainSegments; i++)
	{
		// Calculate sine and cosine of main segment angle
		auto sinMainSegment = sin(currentMainSegmentAngle);
		auto cosMainSegment = cos(currentMainSegmentAngle);
		auto currentTubeSegmentAngle = 0.0f;
		std::vector<glm::vec3> segment_points;
		for (auto j = 0; j < _tubeSegments; j++)
		{
			// Calculate sine and cosine of tube segment angle
			auto sinTubeSegment = sin(currentTubeSegmentAngle);
			auto cosTubeSegment = cos(currentTubeSegmentAngle);

			// Calculate vertex position on the surface of torus
			auto surfacePosition = glm::vec3(
				(_mainRadius + _tubeRadius * cosTubeSegment) * cosMainSegment,
				(_mainRadius + _tubeRadius * cosTubeSegment) * sinMainSegment,
				_tubeRadius * sinTubeSegment);

			//vertex_list.push_back(surfacePosition);
			segment_points.push_back(surfacePosition);

			// Update current tube angle
			currentTubeSegmentAngle += tubeSegmentAngleStep;
		}
		segments_list.push_back(segment_points);
		segment_points.clear();

		// Update main segment angle
		currentMainSegmentAngle += mainSegmentAngleStep;
	}

	float horizontalStep = 1.0 / _mainSegments;
	float verticalStep = 1.0 / _tubeSegments;
	float u = 0.0;
	float v = 0.0;

	// connect the various segments together, forming triangles
	for (int i = 0; i < _mainSegments; i++)
	{
		for (int j = 0; j < _tubeSegments; j++)
		{
			if (((i + 1) < _mainSegments) && ((j + 1) < _tubeSegments))
			{
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
				vertex_list.push_back(segments_list[i][j + 1]);
				normal = normalize(segments_list[i][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v + verticalStep));
				vertex_list.push_back(segments_list[i + 1][j + 1]);
				normal = normalize(segments_list[i + 1][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v + verticalStep));
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
				vertex_list.push_back(segments_list[i + 1][j]);
				normal = normalize(segments_list[i + 1][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v));
				vertex_list.push_back(segments_list[i + 1][j + 1]);
				normal = normalize(segments_list[i + 1][j + 1] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u + horizontalStep, v - verticalStep));
				vertex_list.push_back(segments_list[i][j]);
				normal = normalize(segments_list[i][j] - center);
				normals_list.push_back(normal);
				texture_coords.push_back(glm::vec2(u, v));
			}
			else
			{
				if (((i + 1) == _mainSegments) && ((j + 1) == _tubeSegments))
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][0]);
					normal = normalize(segments_list[i][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, 0));
					vertex_list.push_back(segments_list[0][0]);
					normal = normalize(segments_list[0][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[0][j]);
					normal = normalize(segments_list[0][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v));
					vertex_list.push_back(segments_list[0][0]);
					normal = normalize(segments_list[0][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}
				else if ((i + 1) == _mainSegments)
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][j + 1]);
					normal = normalize(segments_list[i][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v + verticalStep));
					vertex_list.push_back(segments_list[0][j + 1]);
					normal = normalize(segments_list[0][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v + verticalStep));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[0][j]);
					normal = normalize(segments_list[0][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v));
					vertex_list.push_back(segments_list[0][j + 1]);
					normal = normalize(segments_list[0][j + 1] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(0, v + verticalStep));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}
				else if ((j + 1) == _tubeSegments)
				{
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i][0]);
					normal = normalize(segments_list[i][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, 0));
					vertex_list.push_back(segments_list[i + 1][0]);
					normal = normalize(segments_list[i + 1][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
					vertex_list.push_back(segments_list[i + 1][j]);
					normal = normalize(segments_list[i + 1][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, v));
					vertex_list.push_back(segments_list[i + 1][0]);
					normal = normalize(segments_list[i + 1][0] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u + horizontalStep, 0));
					vertex_list.push_back(segments_list[i][j]);
					normal = normalize(segments_list[i][j] - center);
					normals_list.push_back(normal);
					texture_coords.push_back(glm::vec2(u, v));
				}

			}
			v += verticalStep;
		}
		v = 0.0;
		u += horizontalStep;
	}

	std::vector<GLfloat> combined_values;

	// combine interleaved vertices, normals, and texture coords
	for (int i = 0; i < vertex_list.size(); i++)
	{
		vertex = vertex_list[i];
		normal = normals_list[i];
		text_coord = texture_coords[i];
		combined_values.push_back(vertex.x);
		combined_values.push_back(vertex.y);
		combined_values.push_back(vertex.z);
		combined_values.push_back(normal.x);
		combined_values.push_back(normal.y);
		combined_values.push_back(normal.z);
		combined_values.push_back(text_coord.x);
		combined_values.push_back(text_coord.y);
	}

	// total float values per each type
	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerUV = 2;

	// store vertex and index count
	mesh.nVertices = vertex_list.size();
	mesh.nIndices = 0;

	// Create VAO
	glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
	glBindVertexArray(mesh.vao);

	// Create VBOs
	glGenBuffers(1, mesh.vbos);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * combined_values.size(), combined_values.data(), GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

	// Create Vertex Attribute Pointers
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
	glEnableVertexAttribArray(2);
}


/* Surface Vertex Shader Source Code*/
const GLchar* surfaceVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 vertexPosition; // VAP position 0 for vertex position data
	layout(location = 1) in vec3 vertexNormal; // VAP position 1 for normals
	layout(location = 2) in vec2 textureCoordinate;

	out vec3 vertexFragmentNormal; // For outgoing normals to fragment shader
	out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
	out vec2 vertexTextureCoordinate;

	//Uniform / Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(vertexPosition, 1.0f); // Transforms vertices into clip coordinates

		vertexFragmentPos = vec3(model * vec4(vertexPosition, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

		vertexFragmentNormal = mat3(transpose(inverse(model))) * vertexNormal; // get normal vectors in world space only and exclude normal translation properties
		vertexTextureCoordinate = textureCoordinate;
	}
);
/* Surface Fragment Shader Source Code*/
const GLchar* surfaceFragmentShaderSource = GLSL(440,

	in vec3 vertexFragmentNormal; // For incoming normals
	in vec3 vertexFragmentPos; // For incoming fragment position
	in vec2 vertexTextureCoordinate;

	out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec4 objectColor;
uniform vec3 ambientColor;
uniform vec3 light1Color;
uniform vec3 light1Position;
uniform vec3 light2Color;
uniform vec3 light2Position;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;
uniform bool ubHasTexture;
uniform float ambientStrength = 0.05f; // Set ambient or global lighting strength
uniform float specularIntensity1 = 0.25f;
uniform float highlightSize1 = 8.0f;
uniform float specularIntensity2 = 0.25f;
uniform float highlightSize2 = 8.0f;

void main()
{
	/*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

	//Calculate Ambient lighting
	vec3 ambient = ambientStrength * ambientColor; // Generate ambient light color

	//**Calculate Diffuse lighting**
	vec3 norm = normalize(vertexFragmentNormal); // Normalize vectors to 1 unit
	vec3 light1Direction = normalize(light1Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
	float impact1 = max(dot(norm, light1Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
	vec3 diffuse1 = impact1 * light1Color; // Generate diffuse light color
	vec3 light2Direction = normalize(light2Position - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
	float impact2 = max(dot(norm, light2Direction), 0.0);// Calculate diffuse impact by generating dot product of normal and light
	vec3 diffuse2 = impact2 * light2Color; // Generate diffuse light color

	//**Calculate Specular lighting**
	vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
	vec3 reflectDir1 = reflect(-light1Direction, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.0), highlightSize1);
	vec3 specular1 = specularIntensity1 * specularComponent1 * light1Color;
	vec3 reflectDir2 = reflect(-light2Direction, norm);// Calculate reflection vector
	//Calculate specular component
	float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);
	vec3 specular2 = specularIntensity2 * specularComponent2 * light2Color;

	//**Calculate phong result**
	//Texture holds the color to be used for all three components
	vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);
	vec3 phong1;
	vec3 phong2;

	if (ubHasTexture == true)
	{
		phong1 = (ambient + diffuse1 + specular1) * textureColor.xyz;
		phong2 = (ambient + diffuse2 + specular2) * textureColor.xyz;
	}
	else
	{
		phong1 = (ambient + diffuse1 + specular1) * objectColor.xyz;
		phong2 = (ambient + diffuse2 + specular2) * objectColor.xyz;
	}

	fragmentColor = vec4(phong1 + phong2, 1.0); // Send lighting results to GPU
	//fragmentColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
);

/* Light Object Shader Source Code*/
const GLchar* lightVertexShaderSource = GLSL(330,
	layout(location = 0) in vec3 aPos;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	}
);

/* Light Object Shader Source Code*/
const GLchar* lightFragmentShaderSource = GLSL(330,
	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(1.0); // set all 4 vector values to 1.0
	}
);

/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
	layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
	layout(location = 2) in vec2 textureCoordinate;  // Color data from Vertex Attrib Pointer 1

	out vec2 vertexTextureCoordinate; // variable to transfer color data to the fragment shader

	//Global variables for the  transform matrices
	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);
		vertexTextureCoordinate = textureCoordinate; // references incoming color data
	}
	);

/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
	in vec2 vertexTextureCoordinate;

	out vec4 fragmentColor;

	uniform sampler2D uTextureBase;
	uniform sampler2D uTextureExtra;
	
	void main()
	{
		fragmentColor = texture(uTextureBase, vertexTextureCoordinate);
	}
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}

int main(int argc, char* argv[])
{
	if (!UInitialize(argc, argv, &gWindow))
		return EXIT_FAILURE;

	// Create the mesh
	//UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object
	CreateMeshes();

	// Create the shader program
	if (!UCreateShaderProgram(surfaceVertexShaderSource, surfaceFragmentShaderSource, gSurfaceProgramId))
		return EXIT_FAILURE;

	if (!UCreateShaderProgram(lightVertexShaderSource, lightFragmentShaderSource, gLightProgramId))
		return EXIT_FAILURE;

	// Load textures
	const char* texFilename = "PaintedWood007C_1K-JPG_Color.jpg";
	if (!UCreateTexture(texFilename, gTextureWood))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "Wood049_4K-PNG_Color.png";
	if (!UCreateTexture(texFilename, gTextureDesk))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "Plastic006.png";
	if (!UCreateTexture(texFilename, gTextureMug))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "Metal032.png";
	if (!UCreateTexture(texFilename, gTextureChemex))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gSurfaceProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTexture"), 0);
	glUniform2f(glGetUniformLocation(gSurfaceProgramId, "uvScale"), gUVScale.x, gUVScale.y);
    // We set the texture as texture unit 1
    glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTexture2"), 1);
	glUniform2f(glGetUniformLocation(gSurfaceProgramId, "uvScale"), gUVScale.x, gUVScale.y);
	// We set the texture as texture unit 2
	glUniform1i(glGetUniformLocation(gSurfaceProgramId, "uTexture3"), 2);
	glUniform2f(glGetUniformLocation(gSurfaceProgramId, "uvScale"), gUVScale.x, gUVScale.y);

	// Sets the background color of the window 
	glClearColor(0.8f, 0.8f, 0.8f, 0.8f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(gWindow))
	{
		// per-frame timing
		// --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// input
		// -----
		UProcessInput(gWindow);

		// Render this frame
		URender();

		glfwPollEvents();
	}

	// Release mesh data
	//UDestroyMesh(gMesh);
	DestroyMeshes();

	//Release texture
	UDestroyTexture(gTextureWood);
	UDestroyTexture(gTextureDesk);
	UDestroyTexture(gTextureMug);
	UDestroyTexture(gTextureChemex);

	// Release shader program
	//UDestroyShaderProgram(gProgramId);
	UDestroyShaderProgram(gSurfaceProgramId);
	UDestroyShaderProgram(gLightProgramId);

	exit(EXIT_SUCCESS); // Terminates the program successfully
}

// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
	// GLFW: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// GLFW: window creation
	// ---------------------
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLEW: initialize
	// ----------------
	// Note: if using GLEW version 1.13 or earlier
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
	static const float cameraSpeed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		perspective = true;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		perspective = false;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	gCamera.ProcessMouseScroll(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
	{
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
	{
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;
	}
	break;

	case GLFW_MOUSE_BUTTON_RIGHT:
	{
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
	break;

	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}
}

// Functioned called to render a frame
void URender()
{
	GLuint uTextureLoc;
	GLuint uTextureLoc2;

	const int nrows = 10;
	const int ncols = 10;
	const int nlevels = 10;

	const float xsize = 10.0f;
	const float ysize = 10.0f;
	const float zsize = 10.0f;

	GLint modelLoc;
	GLint viewLoc;
	GLint projLoc;
	GLint viewPosLoc;
	GLint ambStrLoc;
	GLint ambColLoc;
	GLint light1ColLoc;
	GLint light1PosLoc;
	GLint light2ColLoc;
	GLint light2PosLoc;
	GLint objColLoc;
	GLint specInt1Loc;
	GLint highlghtSz1Loc;
	GLint specInt2Loc;
	GLint highlghtSz2Loc;
	GLint uHasTextureLoc;
	bool ubHasTextureVal;
	glm::mat4 scale;
	glm::mat4 rotation;
	glm::mat4 translation;
	glm::mat4 model;
	//glm::mat4 view;
	//glm::mat4 projection;

	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// Clear the frame and z buffers
	glClearColor(0.8f, 0.8f, 0.8f, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera / view transformation
	glm::mat4 view = gCamera.GetViewMatrix();

	// Creates a perspective projection
	glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

	//change to perspective
	auto aspectRatio = (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT;

	if (!perspective)
	{
		// p for perspective (default)
		glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, -100.0f);
	}
	else
		// o for ortho
		projection = glm::ortho(-aspectRatio, aspectRatio, -10.0f, 10.0f, 5.1f, 50.0f);
	// Set the shader to be used
	glUseProgram(gSurfaceProgramId);


	GLint objectColorLoc = glGetUniformLocation(gSurfaceProgramId, "uObjectColor");

		// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gSurfaceProgramId, "model");
	viewLoc = glGetUniformLocation(gSurfaceProgramId, "view");
	projLoc = glGetUniformLocation(gSurfaceProgramId, "projection");
	viewPosLoc = glGetUniformLocation(gSurfaceProgramId, "viewPosition");
	ambStrLoc = glGetUniformLocation(gSurfaceProgramId, "ambientStrength");
	ambColLoc = glGetUniformLocation(gSurfaceProgramId, "ambientColor");
	light1ColLoc = glGetUniformLocation(gSurfaceProgramId, "light1Color");
	light1PosLoc = glGetUniformLocation(gSurfaceProgramId, "light1Position");
	light2ColLoc = glGetUniformLocation(gSurfaceProgramId, "light2Color");
	light2PosLoc = glGetUniformLocation(gSurfaceProgramId, "light2Position");
	objColLoc = glGetUniformLocation(gSurfaceProgramId, "objectColor");
	specInt1Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity1");
	highlghtSz1Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize1");
	specInt2Loc = glGetUniformLocation(gSurfaceProgramId, "specularIntensity2");
	highlghtSz2Loc = glGetUniformLocation(gSurfaceProgramId, "highlightSize2");
	uHasTextureLoc = glGetUniformLocation(gSurfaceProgramId, "ubHasTexture");
	uTextureLoc = glGetUniformLocation(gSurfaceProgramId, "uTexture");
	uTextureLoc2 = glGetUniformLocation(gSurfaceProgramId, "uTexture2");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the camera view location
	glUniform3f(viewPosLoc, gCamera.Position.x, gCamera.Position.y, gCamera.Position.z);
	//set ambient lighting strength
	glUniform1f(ambStrLoc, 0.2f);
	//set ambient color
	glUniform3f(ambColLoc, 1.1f, 1.1f, 1.1f);
	glUniform3f(light1ColLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light1PosLoc, 1.0f, 5.0f, 8.8f);
	glUniform3f(light2ColLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(light2PosLoc, 1.0f, 5.0f, -7.0f);
	//set specular intensity
	glUniform1f(specInt1Loc, .2f);
	glUniform1f(specInt2Loc, .2f);
	//set specular highlight size
	glUniform1f(highlghtSz1Loc, 1.0f);
	glUniform1f(highlghtSz2Loc, 1.0f);

	ubHasTextureVal = true;
	glUniform1i(uHasTextureLoc, ubHasTextureVal);

	///////////////// -- PLANE -- ////////////////////
	
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gPlaneMesh.vao);

	// 1. Scales the plane
	scale = glm::scale(glm::vec3(8.0f, 1.0f, 6.0f));
	// 2. Rotate the plane
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	// 3. Position the plane
	translation = glm::translate(glm::vec3(0.0f, -0.5f, 0.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureDesk);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);


	///////////////// -- LOWER BOX -- ////////////////////

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gBoxLowerMesh.vao);

		// 1. Scales the lower half of box
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// 2. Rotate the lower half of box
	rotation = glm::rotate(-0.30f, glm::vec3(0.0, 0.025f, 0.0f));
	// 3. Position the lower half of box
	translation = glm::translate(glm::vec3(4.5f, 0.0f, -2.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 234.0f / 255.0f, 221.0f/255.0f, 202.0f / 255.0f, 1.0f);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureWood);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, gBoxUpperMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	///////////////// -- UPPER BOX -- ////////////////////

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gBoxUpperMesh.vao);

	// 1. Scales the upper half of box
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	// 2. Rotate the upper half of box
	rotation = glm::rotate(-0.30f, glm::vec3(0.0, 0.025f, 0.0f));
	// 3. Position the upper half of box
	translation = glm::translate(glm::vec3(4.5f, 0.6f, -2.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureWood);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, gBoxUpperMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	///////////////// -- MUG -- ////////////////////

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gMugMesh.vao);

	// 1. Scales the mug
	scale = glm::scale(glm::vec3(0.75f, 2.0f, 0.75f));
	// 2. Rotate the mug
	rotation = glm::rotate(-0.30f, glm::vec3(0.0, 0.025f, 0.0f));
	// 3. Position the mug
	translation = glm::translate(glm::vec3(-5.0f, 0.0f, -0.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureMug);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	///////////////// -- MUG HANDLE -- ////////////////////
	// 
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gHandleMesh.vao);

	// 1. Scales the cone
	scale = glm::scale(glm::vec3(0.5f, 0.5f, 3.0f));
	// 2. Rotate the cone
	rotation = glm::rotate(1.5f, glm::vec3(0.0f, 0.0f, -2.0f));
	// 3. Position the cone
	translation = glm::translate(glm::vec3(-4.3f, 1.0f, -0.8f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureMug);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gHandleMesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	///////////////// --CHEMEX -- ////////////////////

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gLowerChemexMesh.vao);

	// 1. Scales the cone
	scale = glm::scale(glm::vec3(1.5f, 1.75f, 1.5f));
	// 2. Rotate the cone
	rotation = glm::rotate(-0.30f, glm::vec3(0.0, 0.025f, 0.0f));
	// 3. Position the cone
	translation = glm::translate(glm::vec3(-2.0f, 0.0f, -1.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureChemex);

	// Draws the triangles
	//glDrawElements(GL_TRIANGLES, gUpperChemexMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_STRIP, 36, 108);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gUpperChemexMesh.vao);

	// 1. Scales the cone
	scale = glm::scale(glm::vec3(1.5f, 1.75f, 1.5f));
	// 2. Rotate the cone
	rotation = glm::rotate(-3.15f, glm::vec3(0.0, 0.0f, 1.0f));
	// 3. Position the cone
	translation = glm::translate(glm::vec3(-2.0f, 3.0f, -1.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureChemex);

	// Draws the triangles
	//glDrawElements(GL_TRIANGLES, gUpperChemexMesh.nIndices, GL_UNSIGNED_INT, (void*)0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_STRIP, 36, 108);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	///////////////// -- CHEMEX COLLAR -- ////////////////////
	// 
	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gChemexCollar.vao);

	// 1. Scales the cone
	scale = glm::scale(glm::vec3(0.5f, 0.5f, 3.0f));
	// 2. Rotate the cone
	rotation = glm::rotate(1.5f, glm::vec3(1.0, 0.0f, 0.0f));
	// 3. Position the cone
	translation = glm::translate(glm::vec3(-2.0f, 1.5f, -1.5f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureWood);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, gChemexCollar.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	///////////////// -- Perfume Jar -- ////////////////////

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gPerfumeMesh.vao);

	// 1. Scales the Jar
	scale = glm::scale(glm::vec3(0.4f, 4.0f, 0.2f));
	// 2. Rotate the Jar
	rotation = glm::rotate(-0.0f, glm::vec3(0.0, 0.025f, 0.0f));
	// 3. Position the Jar
	translation = glm::translate(glm::vec3(1.5f, 0.0f, -2.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//glProgramUniform4f(gSurfaceProgramId, objectColorLoc, 234.0f / 255.0f, 221.0f/255.0f, 202.0f / 255.0f, 1.0f);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureChemex);

	// Draws the triangles
	glDrawElements(GL_TRIANGLES, gPerfumeMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);

	///////////////// -- PERFUME CAP -- ////////////////////

	// Activate the VBOs contained within the mesh's VAO
	glBindVertexArray(gCapMesh.vao);

	// 1. Scales the cap
	scale = glm::scale(glm::vec3(0.25f, 0.5f, 0.25f));
	// 2. Rotate the cap
	rotation = glm::rotate(-0.30f, glm::vec3(0.0, 0.025f, 0.0f));
	// 3. Position the cap
	translation = glm::translate(glm::vec3(1.5f, 1.0f, -2.0f));
	// Model matrix: transformations are applied right-to-left order
	model = translation * rotation * scale;
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureMug);

	// Draws the triangles
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUniform4f(objColLoc, 0.5f, 0.5f, 0.5f, 1.0f);


	// Set the shader to be used
	glUseProgram(gLightProgramId);

	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gLightProgramId, "model");
	viewLoc = glGetUniformLocation(gLightProgramId, "view");
	projLoc = glGetUniformLocation(gLightProgramId, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}

void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}


void UDestroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}