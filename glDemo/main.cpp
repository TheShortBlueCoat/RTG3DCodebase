
#include "core.h"
#include "TextureLoader.h"
#include "ArcballCamera.h"
#include "GUClock.h"
#include "PrincipleAxes.h"
#include "shader_setup.h"
#include "helper.h"
#include "AIMesh.h"
#include "Cube.h"
#include "Scene.h"


using namespace std;
using namespace glm;


#pragma region Global variables

GUClock* g_gameClock = nullptr;

// Mouse tracking
bool				g_mouseDown = false;
double				g_prevMouseX, g_prevMouseY;

// Global Example objects
// shouldn't really be anything in here for the final submission
ArcballCamera* g_mainCamera = nullptr;
CGPrincipleAxes* g_principleAxes = nullptr;
Cube* g_cube = nullptr;

GLuint g_flatColourShader;

GLuint g_texDirLightShader;
vec3 g_DLdirection = vec3(0.0f, 1.0f, 0.0f);
vec3 g_DLcolour = vec3(1.0f, 1.0f, 1.0f);
vec3 g_DLambient = vec3(0.2f, 0.2f, 0.2f);

AIMesh* g_creatureMesh = nullptr;
vec3 g_beastPos = vec3(2.0f, 0.0f, 0.0f);
float g_beastRotation = 0.0f;


AIMesh* g_mapMesh = nullptr;
vec3 g_mapPos = vec3(2.0f, -3.0f, 0.0f);
float g_mapRotation = 0.0f;

AIMesh* g_wall1Mesh = nullptr;
vec3 g_wall1Pos = vec3(2.0f, -3.0f, 0.0f);
float g_wall1Rotation = 0.0f;

AIMesh* g_wall2Mesh = nullptr;
vec3 g_wall2Pos = vec3(2.0f, -3.0f, 0.0f);
float g_wall2Rotation = 0.0f;

AIMesh* g_wall3Mesh = nullptr;
vec3 g_wall3Pos = vec3(2.0f, -3.0f, 0.0f);
float g_wall3Rotation = 0.0f;

AIMesh* g_wall4Mesh = nullptr;
vec3 g_wall4Pos = vec3(2.0f, -3.0f, 0.0f);
float g_wall4Rotation = 0.0f;

AIMesh* g_torchMesh = nullptr;
vec3 g_torchPos = vec3(2.0f, -3.0f, 0.0f);
float g_torchRotation = 0.0f;

AIMesh* g_ghostMesh = nullptr;
vec3 g_ghostPos = vec3(2.0f, 0.0f, 0.0f);
float g_ghostRotation = 0.0f;

int g_showing = 0;
int g_NumExamples = 2;

//Global Game Object
Scene* g_Scene = nullptr;

// Window size
const unsigned int g_initWidth = 512;
const unsigned int g_initHeight = 512;

#pragma endregion


// Function prototypes
void renderScene();
void updateScene();
void resizeWindow(GLFWwindow* _window, int _width, int _height);
void keyboardHandler(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
void mouseMoveHandler(GLFWwindow* _window, double _xpos, double _ypos);
void mouseButtonHandler(GLFWwindow* _window, int _button, int _action, int _mods);
void mouseScrollHandler(GLFWwindow* _window, double _xoffset, double _yoffset);
void mouseEnterHandler(GLFWwindow* _window, int _entered);


int main()
{
	//
	// 1. Initialisation
	//

	g_gameClock = new GUClock();

#pragma region OpenGL and window setup

	// Initialise glfw and setup window
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_TRUE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);

	GLFWwindow* window = glfwCreateWindow(g_initWidth, g_initHeight, "GDV5001", NULL, NULL);

	// Check window was created successfully
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Set callback functions to handle different events
	glfwSetFramebufferSizeCallback(window, resizeWindow); // resize window callback
	glfwSetKeyCallback(window, keyboardHandler); // Keyboard input callback
	glfwSetCursorPosCallback(window, mouseMoveHandler);
	glfwSetMouseButtonCallback(window, mouseButtonHandler);
	glfwSetScrollCallback(window, mouseScrollHandler);
	glfwSetCursorEnterCallback(window, mouseEnterHandler);

	// Initialise glew
	glewInit();


	// Setup window's initial size
	resizeWindow(window, g_initWidth, g_initHeight);

#pragma endregion

	// Initialise scene - geometry and shaders etc
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // setup background colour to be black
	glClearDepth(1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//
	// Setup the Example Objects
	//

	g_texDirLightShader = setupShaders(string("Assets\\Shaders\\texture-directional.vert"), string("Assets\\Shaders\\texture-directional.frag"));
	g_flatColourShader = setupShaders(string("Assets\\Shaders\\flatColour.vert"), string("Assets\\Shaders\\flatColour.frag"));

	g_mainCamera = new ArcballCamera(0.0f, 0.0f, 1.98595f, 55.0f, 1.0f, 0.1f, 500.0f);

	g_principleAxes = new CGPrincipleAxes();

	g_cube = new Cube();

	g_creatureMesh = new AIMesh(string("Assets\\beast\\beast.obj"));
	if (g_creatureMesh) {
		g_creatureMesh->addTexture(string("Assets\\beast\\beast_texture.bmp"), FIF_BMP);
	}

	g_mapMesh = new AIMesh(string("Assets\\map\\cube1.obj"));
	if (g_mapMesh) {
		g_mapMesh->addTexture(string("Assets\\map\\map_texture.bmp"), FIF_BMP);
	}

	g_wall1Mesh = new AIMesh(string("Assets\\map\\cube1.obj"));
	if (g_wall1Mesh) {
		g_wall1Mesh->addTexture(string("Assets\\map\\map_texture.bmp"), FIF_BMP);
	}

	g_wall2Mesh = new AIMesh(string("Assets\\map\\cube1.obj"));
	if (g_wall2Mesh) {
		g_wall2Mesh->addTexture(string("Assets\\map\\map_texture.bmp"), FIF_BMP);
	}

	g_wall3Mesh = new AIMesh(string("Assets\\map\\cube1.obj"));
	if (g_wall3Mesh) {
		g_wall3Mesh->addTexture(string("Assets\\map\\map_texture.bmp"), FIF_BMP);
	}

	g_wall4Mesh = new AIMesh(string("Assets\\map\\cube1.obj"));
	if (g_wall4Mesh) {
		g_wall4Mesh->addTexture(string("Assets\\map\\map_texture.bmp"), FIF_BMP);
	}

	g_torchMesh = new AIMesh(string("Assets\\torch\\torch.obj"));
	if (g_torchMesh) {
		g_torchMesh->addTexture(string("Assets\\torch\\torch_texture.bmp"), FIF_BMP);
	}

	g_ghostMesh = new AIMesh(string("Assets\\ghost\\ghost.obj"));
	if (g_ghostMesh) {
		g_ghostMesh->addTexture(string("Assets\\ghost\\ghost_texture.bmp"), FIF_BMP);
	}

	//
	//Set up Scene class
	//

	g_Scene = new Scene();

	ifstream manifest;
	manifest.open("manifest.txt");

	g_Scene->Load(manifest);
	g_Scene->Init();

	manifest.close();


	//
	// Main loop
	// 

	while (!glfwWindowShouldClose(window))
	{
		updateScene();
		renderScene();						// Render into the current buffer
		glfwSwapBuffers(window);			// Displays what was just rendered (using double buffering).

		glfwPollEvents();					// Use this version when animating as fast as possible

		// update window title
		char timingString[256];
		sprintf_s(timingString, 256, "CIS5013: Average fps: %.0f; Average spf: %f", g_gameClock->averageFPS(), g_gameClock->averageSPF() / 1000.0f);
		glfwSetWindowTitle(window, timingString);
	}

	glfwTerminate();

	if (g_gameClock)
	{
		g_gameClock->stop();
		g_gameClock->reportTimingData();
	}

	return 0;
}


// renderScene - function to render the current scene
void renderScene()
{
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 cameraTransform = g_mainCamera->projectionTransform() * g_mainCamera->viewTransform();

	mat4 cameraProjection = g_mainCamera->projectionTransform();
	mat4 cameraView = g_mainCamera->viewTransform() * translate(identity<mat4>(), -g_beastPos);

#// Render principle axes - no modelling transforms so just use cameraTransform
	if (true)
	{
		// Render axes 
		glUseProgram(g_flatColourShader);
		GLint pLocation;
		Helper::SetUniformLocation(g_flatColourShader, "viewMatrix", &pLocation);
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
		Helper::SetUniformLocation(g_flatColourShader, "projMatrix", &pLocation);
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
		Helper::SetUniformLocation(g_flatColourShader, "modelMatrix", &pLocation);
		mat4 modelTransform = identity<mat4>();
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

		g_principleAxes->render();
	}

	switch (g_showing)
	{
	case 0:
	{
		glUseProgram(g_texDirLightShader);

		GLint pLocation;
		Helper::SetUniformLocation(g_texDirLightShader, "viewMatrix", &pLocation);
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraView);
		Helper::SetUniformLocation(g_texDirLightShader, "projMatrix", &pLocation);
		glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&cameraProjection);
		Helper::SetUniformLocation(g_texDirLightShader, "texture", &pLocation);
		glUniform1i(pLocation, 0); // set to point to texture unit 0 for AIMeshes
		Helper::SetUniformLocation(g_texDirLightShader, "DIRDir", &pLocation);
		glUniform3fv(pLocation, 1, (GLfloat*)&g_DLdirection);
		Helper::SetUniformLocation(g_texDirLightShader, "DIRCol", &pLocation);
		glUniform3fv(pLocation, 1, (GLfloat*)&g_DLcolour);
		Helper::SetUniformLocation(g_texDirLightShader, "DIRAmb", &pLocation);
		glUniform3fv(pLocation, 1, (GLfloat*)&g_DLambient);
		if (g_creatureMesh) {

			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_beastPos) * eulerAngleY<float>(glm::radians<float>(g_beastRotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_creatureMesh->setupTextures();
			g_creatureMesh->render();
		}

		if (g_mapMesh) {

			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_mapPos) * eulerAngleY<float>(glm::radians<float>(g_mapRotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_mapMesh->setupTextures();
			g_mapMesh->render();
		}

		if (g_wall1Mesh) {

			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_wall1Pos) * eulerAngleY<float>(glm::radians<float>(g_wall1Rotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_wall1Mesh->setupTextures();
			g_wall1Mesh->render();
		}

		if (g_wall2Mesh) {

			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_wall2Pos) * eulerAngleY<float>(glm::radians<float>(g_wall2Rotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_wall2Mesh->setupTextures();
			g_wall2Mesh->render();
		}

		if (g_wall3Mesh) {
			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_wall3Pos) * eulerAngleY<float>(glm::radians<float>(g_wall3Rotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_wall3Mesh->setupTextures();
			g_wall3Mesh->render();
		}

		if (g_wall4Mesh) {
			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_wall4Pos) * eulerAngleY<float>(glm::radians<float>(g_wall4Rotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_wall4Mesh->setupTextures();
			g_wall4Mesh->render();
		}

		if (g_torchMesh) {
			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_torchPos) * eulerAngleY<float>(glm::radians<float>(g_torchRotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			g_torchMesh->setupTextures();
			g_torchMesh->render();
		}
		
		if (g_ghostMesh) {
			// Enable blending for transparency
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Set the alpha value for the ghost
			GLint alphaLocation;
			Helper::SetUniformLocation(g_texDirLightShader, "alpha", &alphaLocation);
			glUniform1f(alphaLocation, 0.5f); // Set alpha to 50% transparency

			// Setup transforms
			Helper::SetUniformLocation(g_texDirLightShader, "modelMatrix", &pLocation);
			mat4 modelTransform = glm::translate(identity<mat4>(), g_ghostPos) * eulerAngleY<float>(glm::radians<float>(g_ghostRotation));
			glUniformMatrix4fv(pLocation, 1, GL_FALSE, (GLfloat*)&modelTransform);

			// Render the ghost
			g_ghostMesh->setupTextures();
			g_ghostMesh->render();

			// Disable blending after rendering
			glDisable(GL_BLEND);
		}
	}
	break;
	case 1:
		g_Scene->Render();
	}

}


// Function called to animate elements in the scene
void updateScene() 
{
	float tDelta = 0.0f;

	if (g_gameClock) {

		g_gameClock->tick();
		tDelta = (float)g_gameClock->gameTimeDelta();
	}

	g_Scene->Update(tDelta);
	//update this
}


#pragma region Event handler functions
//none of this is currently passed to the Game object
//probably a good idea to do that

// Function to call when window resized
void resizeWindow(GLFWwindow* _window, int _width, int _height)
{
	if (g_mainCamera) {

		g_mainCamera->setAspect((float)_width / (float)_height);
	}

	glViewport(0, 0, _width, _height);		// Draw into entire window
}


// Function to call to handle keyboard input
void keyboardHandler(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	if (_action == GLFW_PRESS) {

		// check which key was pressed...
		switch (_key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(_window, true);
			break;

		case GLFW_KEY_SPACE:
			g_showing++;
			g_showing = g_showing % g_NumExamples;
			break;
		case GLFW_KEY_K:
			g_Scene->NextCamera();
			break;
		case GLFW_KEY_W:
			if (g_showing == 1) g_Scene->Forward();
			break;
		case GLFW_KEY_A:
			if (g_showing == 1) g_Scene->Left();
			break;
		case GLFW_KEY_S:
			if (g_showing == 1) g_Scene->Backward();
			break;
		case GLFW_KEY_D:
			if (g_showing == 1) g_Scene->Right();
			break;
		case GLFW_KEY_Q:
			if (g_showing == 1) g_Scene->Up();
			break;
		case GLFW_KEY_E:
			if (g_showing == 1) g_Scene->Down();
			break;
		default:
		{
		}
		}
	}
	else if (_action == GLFW_RELEASE) 
	{
		// handle key release events
		switch (_key)
		{
		default:
		{
		}
		}
	}
}


void mouseMoveHandler(GLFWwindow* _window, double _xpos, double _ypos) 
{
	if (g_mouseDown) {

		//float tDelta = gameClock->gameTimeDelta();

		float dx = float(_xpos - g_prevMouseX);// *360.0f * tDelta;
		float dy = float(_ypos - g_prevMouseY);// *360.0f * tDelta;

		if (g_mainCamera)
			g_mainCamera->rotateCamera(-dy, -dx);

		g_prevMouseX = _xpos;
		g_prevMouseY = _ypos;

	}
}

void mouseButtonHandler(GLFWwindow* _window, int _button, int _action, int _mods) 
{
	if (_button == GLFW_MOUSE_BUTTON_LEFT) 
	{
		if (_action == GLFW_PRESS) 
		{
			g_mouseDown = true;
			glfwGetCursorPos(_window, &g_prevMouseX, &g_prevMouseY);
		}
		else if (_action == GLFW_RELEASE) 
		{
			g_mouseDown = false;
		}
	}
}

void mouseScrollHandler(GLFWwindow* _window, double _xoffset, double _yoffset) {

	if (g_mainCamera) 
	{
		if (_yoffset < 0.0)
			g_mainCamera->scaleRadius(1.1f);
		else if (_yoffset > 0.0)
			g_mainCamera->scaleRadius(0.9f);
	}
}

void mouseEnterHandler(GLFWwindow* _window, int _entered) 
{
}

#pragma endregion