//main.cpp Project
#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 1680;
int glWindowHeight = 1050;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

GLfloat yaw   = -90.0f;    // unghi inițial (ca să privim pe axa -Z)
GLfloat pitch =  0.0f;     // inițial camera e pe orizontală
GLfloat lastX = (float)glWindowWidth  / 2.0;
GLfloat lastY = (float)glWindowHeight / 2.0;
bool firstMouse = true; // pentru a evita saltul inițial de mouse

// matrices
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

// light parameters
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

// camera
gps::Camera myCamera(
	glm::vec3(0.0f, 1.0f, 13.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D myBoat;
gps::Model3D myBoat2;
gps::Model3D myScene;
gps::Model3D myBlock;
gps::Model3D myWater;
gps::Model3D screenQuad;
GLfloat angle;

// skybox
gps::SkyBox mySkyBox;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader depthShader;
gps::Shader waterShader;
gps::Shader screenQuadShader;

// to change color and background
bool setLightColor = false;
bool setSunsetSkyBox = false;
bool setNight = false;

// to change the fog
bool fogOn = false;
GLfloat fogDensity = 0.01f;
GLfloat lastFogDensity = 0.01f;
glm::vec4 fogColor;
GLuint fogLoc;
GLuint fogColorLoc;

// spotlight
GLfloat constant = 0.3f;
GLfloat linear = 0.1f;
GLfloat quadratic = 0.1f;
glm::vec3 position = glm::vec3(-0.70f, 3.0f, -1.0f);
glm::vec3 lampColor;

GLuint lampColorLoc;
GLuint constantLoc;
GLuint linearLoc;
GLuint quadraticLoc;
GLuint positionLoc;

// angles
GLfloat spotCutoff = 0.0f;
GLfloat spotOuterCutoff = 0.0f;

// --------------------------------------------
// Presentation mode
// --------------------------------------------
bool animationActive = false;

float radius = 0;

float presentationSpeed = 0.2f;
float presentationAngle = 0.0f;

float initialRadius = 25.0f;
float decayRate = 0.002f;


// view mode
int mode = 0;

float globalScale = 2.0f;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void initSkybox() {
	std::vector<const GLchar*> faces;
	if (setSunsetSkyBox == false) {
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/skybox/right.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/skybox/left.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/skybox/top.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/skybox/bottom.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/skybox/back.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/skybox/front.tga");
	}
	else {
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/sunsetskybox/marslike01rt.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/sunsetskybox/marslike01lf.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/sunsetskybox/marslike01up.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/sunsetskybox/marslike01dn.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/sunsetskybox/marslike01bk.tga");
		faces.push_back("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/sunsetskybox/marslike01ft.tga");
	}
	mySkyBox.Load(faces);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	// Toggle the light color
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		setLightColor = !setLightColor;
		setSunsetSkyBox = !setSunsetSkyBox;
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}

	if (pressedKeys[GLFW_KEY_F])
	{
		if (!fogOn) {
			fogOn = true;
			fogDensity = lastFogDensity;
		}
		else {
			fogOn = false;
			lastFogDensity = fogDensity;
			fogDensity = 0.0f;
		}
	}

	if (pressedKeys[GLFW_KEY_G]) {
		if (fogOn && fogDensity < 1.0f) {
			fogDensity += 0.02f;
		}
	}

	if (pressedKeys[GLFW_KEY_H]) {
		if (fogOn && fogDensity > 0.02f) {
			fogDensity -= 0.02f;
		}
	}

	if (pressedKeys[GLFW_KEY_P]) {
		animationActive = !animationActive;

		if (animationActive == true) {
			gps::Camera aux(
				glm::vec3(11.516719f, 11.220733f, -13.750375f),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			myCamera = aux;
			view = myCamera.getViewMatrix();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}
	}

	if (pressedKeys[GLFW_KEY_N]) {
		setNight = !setNight;
	}

	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPointSize(2.0f);
	}

	if (pressedKeys[GLFW_KEY_UP]) {
		presentationSpeed += 0.05f; // Crește viteza
	}
	if (pressedKeys[GLFW_KEY_DOWN]) {
		presentationSpeed = std::max(0.0f, presentationSpeed - 1.0f); // Scade viteza, fără valori negative
	}
	if (pressedKeys[GLFW_KEY_LEFT]) {
		initialRadius = std::max(1.0f, radius - 0.5f); // Scade raza
	}
	if (pressedKeys[GLFW_KEY_RIGHT]) {
		initialRadius += 0.5f; // Crește raza
	}



}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (animationActive == false) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		double xoffset = xpos - lastX;
		double yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		double sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		myCamera.rotate(pitch, yaw);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

}

void processMovement() {
    if (animationActive) {
    	// 1. Creștem unghiul cu "presentationSpeed"
    	presentationAngle += presentationSpeed;  // unghi în grade

    	// 2. Calculez o rază care scade ușor în timp (opțional)
    	float radius = initialRadius * exp(-decayRate * presentationAngle);

    	// 3. Definim un punct-țintă (ex: centrul scenei)
    	glm::vec3 targetPoint(-0.39f, 0.53f, -3.83f);

    	// 4. Poziția camerei se calculează cu trigonometrie
    	float camX = targetPoint.x + radius * cos(glm::radians(presentationAngle));
    	float camZ = targetPoint.z + radius * sin(glm::radians(presentationAngle));
    	float camY = targetPoint.y + 0.5f;

        glm::vec3 cameraPosition = glm::vec3(camX, camY, camZ);

        // Punctul țintă al camerei (centrul scenei)


        // Actualizează camera
        myCamera.setPosition(cameraPosition);
        myCamera.setTarget(targetPoint);

        // Actualizează matricea de vizualizare
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Recalculează normalMatrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    } else {
        // Control manual al camerei
        if (pressedKeys[GLFW_KEY_W]) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_S]) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_A]) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_D]) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        }

        // Actualizează matricea de vizualizare
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Recalculează normalMatrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window scaling pentru HiDPI
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// Antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);

	// **Înregistrăm mouseCallback** și ascundem cursorul
	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);
	glfwSwapInterval(1);

#if not defined (__APPLE__)
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// Informații context
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void initObjects() {
	myScene.LoadModel("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/m_scene/scene.obj");
	myBoat.LoadModel("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/m_boat/boat.obj");
	myBoat2.LoadModel("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/m_boat/boat2.obj");
	myBlock.LoadModel("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/m_cube/cube.obj");
	myWater.LoadModel("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/m_scene/water.obj");
	screenQuad.LoadModel("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/quad/quad.obj");
}

void initShaders() {
	myBasicShader.loadShader("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/basic.vert", "/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/basic.frag");
	myBasicShader.useShaderProgram();
	skyboxShader.loadShader("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/skyboxShader.vert", "/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	depthShader.loadShader("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/depth.vert", "/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/depth.frag");
	depthShader.useShaderProgram();
	waterShader.loadShader("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/water.vert", "/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/water.frag");
	waterShader.useShaderProgram();
	screenQuadShader.loadShader("/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/screenQuad.vert", "/Users/ly/Documents/University/AN3/S5/(PG)PrelucrareGrafica/PG_L/Interactive-3D-Graphics-Scene-main/shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
}

glm::mat4 computeLightSpaceTrMatrix() {
	// Poziția luminii (ca un point light la altitudine 20)
	glm::vec3 lightPos = glm::vec3(10.0f, 20.0f, 10.0f);

	// Creăm view matrix
	glm::mat4 lightView = glm::lookAt(
		lightPos,
		glm::vec3(0.0f),    // Să privească spre origine
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// Definim volumul ortografic (ajustează numeric pentru scena ta)
	float halfWidth = 50.0f;
	float near_plane = 0.1f, far_plane = 2000.0f;
	glm::mat4 lightProjection = glm::ortho(
		-halfWidth, halfWidth,   // left,  right
		-halfWidth, halfWidth,   // bottom,top
		near_plane, far_plane
	);

	// Matricea combinată
	return lightProjection * lightView;
}

void updateSpotlightDirection() {
	float time = glfwGetTime();
	float rotationSpeed = 0.5f;
	glm::vec3 spotDirRotating;
	spotDirRotating.x = sin(time * rotationSpeed);
	spotDirRotating.y = -0.35f;
	spotDirRotating.z = -cos(time * rotationSpeed);
	spotDirRotating = glm::normalize(spotDirRotating);

	// Actualizează uniformul spotDir în shader
	glUseProgram(myBasicShader.shaderProgram);
	GLuint spotDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotDir");
	glUniform3fv(spotDirLoc, 1, glm::value_ptr(spotDirRotating));
}

void initUniforms() {

	myBasicShader.useShaderProgram();

	// create model matrix for myScene
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// compute normal matrix for myScene
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(
		glm::radians(45.0f),
		(float)retina_width / (float)retina_height,
		0.1f,
		1000.0f
	);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 0.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	if (setLightColor == false)
		lightColor = glm::vec3(0.86f, 0.88f, 0.86f); //white light
	else
		lightColor = glm::vec3(0.8f, 0.259f, 0.098f); //orange light
	if (setNight == true)
		lightColor = glm::vec3(0.05f, 0.05f, 0.05f);// black light

	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//set fog
	fogLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
	glUniform1f(fogLoc, fogDensity);//fog

	//set fog color
	if (setLightColor == false)
		fogColor = glm::vec4(1.0f, 1.0f, 0.784f, 1.0f);//white color
	else
		fogColor = glm::vec4(0.89f, 0.45f, 0.09f, 1.0f); //orange color
	if (setNight == true)
		fogColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);// black color


	fogColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogColor");
	glUniform4fv(fogColorLoc, 1, glm::value_ptr(fogColor));

	//set spotlight
	lampColor = glm::vec3(1.0f, 0.92f, 0.97f); //orange light
	lampColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lampColor");
	// send light color to shader
	glUniform3fv(lampColorLoc, 1, glm::value_ptr(lampColor));

	//send constants loc
	constantLoc = glGetUniformLocation(myBasicShader.shaderProgram, "constant");
	glUniform1f(constantLoc, constant);//constant
	linearLoc = glGetUniformLocation(myBasicShader.shaderProgram, "linear");
	glUniform1f(linearLoc, linear);//linear
	quadraticLoc = glGetUniformLocation(myBasicShader.shaderProgram, "quadratic");
	glUniform1f(quadraticLoc, quadratic);//quadratic
	positionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "position");
	glUniform3fv(positionLoc, 1, glm::value_ptr(position));
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	// 1. Definim unghiurile conului (în grade).
	//    spotCutoff e unghiul "interior", spotOuterCutoff e unghiul "exterior"
	spotCutoff = glm::cos(glm::radians(12.0f));      // un "con" relativ strâns
	spotOuterCutoff = glm::cos(glm::radians(17.0f)); // partea mai difuză

	// 2. Definim vectorul de direcție orizontal
	glm::vec3 spotDir = glm::vec3(0.0f, -0.35f, -1.0f); // "bate" pe -Z

	// 3. Luăm locațiile uniformelor
	GLuint spotDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotDir");
	GLuint spotCutOffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotCutoff");
	GLuint spotOuterCutOffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotOuterCutoff");

	// 4. Trimitem datele
	glUniform3fv(spotDirLoc, 1, glm::value_ptr(spotDir));
	glUniform1f(spotCutOffLoc, spotCutoff);
	glUniform1f(spotOuterCutOffLoc, spotOuterCutoff);
}

void initFBO() {
	// 1. Crearea Framebuffer-ului
	glGenFramebuffers(1, &shadowMapFBO);

	// 2. Crearea texturii de adâncime
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 SHADOW_WIDTH, SHADOW_HEIGHT, 0,
				 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// Setarea parametrilor texturii
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// 3. Atașarea texturii de adâncime la framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	// 4. Specificarea că nu avem atașamente de culoare sau stencil
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// 5. Verificarea completeței framebuffer-ului
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Framebuffer not complete!" << std::endl;

	// 6. Dezactivarea framebuffer-ului curent
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawObjects(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();

	glm::mat4 globalScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(globalScale));


	// 1. Desenăm scena statică
	glm::mat4 sceneModelMatrix = glm::mat4(1.0f);

	// => Aplici scalarea globală
	sceneModelMatrix = globalScaleMatrix * sceneModelMatrix;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sceneModelMatrix));

	if(!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * sceneModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	myScene.Draw(shader);

	// 2. Desenăm barca cu animație

	// Parametri animație barcă
	float floatAmplitude = 0.025f;
	float floatFrequency = 1.0f;
	float tiltAmplitude = glm::radians(5.0f);
	float tiltFrequency = 1.0f;

	double currentTime = glfwGetTime();
	float verticalOffset = sin(currentTime * floatFrequency) * floatAmplitude;
	float tiltAngle = sin(currentTime * tiltFrequency) * tiltAmplitude;

	glm::mat4 boatModelMatrix = glm::mat4(1.0f);
	boatModelMatrix = glm::translate(boatModelMatrix, glm::vec3(0.0f, verticalOffset, 0.0f));
	boatModelMatrix = glm::rotate(boatModelMatrix, tiltAngle, glm::vec3(1.0f, 1.0f, 0.0f));

	// => Aplici scalarea globală
	boatModelMatrix = globalScaleMatrix * boatModelMatrix;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(boatModelMatrix));

	if(!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * boatModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	myBoat.Draw(shader);

	// -------------------------------------------------------
	// 3. Animație PENTRU corabia 2 (myBoat2)
	// -------------------------------------------------------
	// Alegem parametri diferiți (ex. altă frecvență, altă amplitudine)
    float floatAmplitude2 = 0.02f;   // se balansează puțin mai mult
    float floatFrequency2 = 1.0f;   // se mișcă mai repede
    float tiltAmplitude2  = glm::radians(4.0f); // se înclină mai mult
    float tiltFrequency2  = 1.2f;

	float verticalOffset2 = sin(currentTime * floatFrequency2) * floatAmplitude2;
	float tiltAngle2 = sin(currentTime * tiltFrequency2) * tiltAmplitude2;

	glm::mat4 boat2ModelMatrix = glm::mat4(1.0f);
	// Poți poziționa a doua corabie mai la stânga/dreapta, de ex:
	boat2ModelMatrix = glm::translate(boat2ModelMatrix, glm::vec3(2.0f, verticalOffset2, -1.0f));
	boat2ModelMatrix = glm::rotate(boat2ModelMatrix, tiltAngle2, glm::vec3(0.0f, 1.0f, 1.0f));

	boat2ModelMatrix = globalScaleMatrix * boat2ModelMatrix;

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(boat2ModelMatrix));
	if(!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * boat2ModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	myBoat2.Draw(shader);

	glm::mat4 blockModelMatrix = glm::mat4(1.0f);
	blockModelMatrix = glm::translate(blockModelMatrix, position);
	blockModelMatrix = glm::scale(blockModelMatrix, glm::vec3(0.1f)); // scale actual
	// position e deja folosită drept “uniform vec3 position” pentru spotlight
	// + eventuala ajustare de scală (să fie un cub mic):
	blockModelMatrix = glm::scale(blockModelMatrix, glm::vec3(0.2f));
	// bloc de 20 cm, să zicem

	// Settezi uniforma modelLoc:
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(blockModelMatrix));
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * blockModelMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	myBlock.Draw(shader);


}

void initWaterUniforms() {
	waterShader.useShaderProgram();

	// Alege culoarea apei în funcție de modul Night
	glm::vec3 waterCol;
	if (setNight) {
		// apă foarte închisă la culoare
		waterCol = glm::vec3(0.01f, 0.01f, 0.01f);
	} else {
		// zi, apă mai albastră
		waterCol = glm::vec3(0.2f, 0.4f, 0.7f);
	}



	// Trimite uniforma waterColor:
	GLint waterColorLoc = glGetUniformLocation(waterShader.shaderProgram, "waterColor");
	glUniform3fv(waterColorLoc, 1, glm::value_ptr(waterCol));

	// Poți lăsa restul parametrelor cum erau
	GLint amplitudeLoc = glGetUniformLocation(waterShader.shaderProgram, "waveAmplitude");
	glUniform1f(amplitudeLoc, 0.045f);

	if (setNight) {
		glUniform1f(amplitudeLoc, 0.025f);
	} else {
		glUniform1f(amplitudeLoc, 0.035f);
	}

	GLint freqLoc = glGetUniformLocation(waterShader.shaderProgram, "waveFrequency");
	glUniform1f(freqLoc, 2.0f);

	// Initializezi time = 0 (dacă vrei), sau nu
	GLint timeLoc = glGetUniformLocation(waterShader.shaderProgram, "time");
	glUniform1f(timeLoc, 0.0f);

	GLint setNightLoc = glGetUniformLocation(waterShader.shaderProgram, "setNight");
	glUniform1i(setNightLoc, setNight ? 1 : 0);
}

void renderWater() {
	waterShader.useShaderProgram();

	// 1. Actualizează uniformele dinamice (timp, matrici, etc.)
	float currentTime = (float)glfwGetTime(); // un exemplu
	GLint timeLoc = glGetUniformLocation(waterShader.shaderProgram, "time");
	glUniform1f(timeLoc, currentTime);

	// trimite și model, view, projection
	glm::mat4 modelWater = glm::mat4(1.0f);
	// dacă vrei să așezi apa la alt nivel (y = 0.3, ex.)
	modelWater = glm::translate(modelWater, glm::vec3(0.0f, 0.0f, 0.0f));

	glm::mat4 globalScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(globalScale));

	modelWater = globalScaleMatrix * modelWater;

	GLint modelLoc = glGetUniformLocation(waterShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelWater));

	glm::mat4 viewWater = myCamera.getViewMatrix();
	GLint viewLoc = glGetUniformLocation(waterShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewWater));

	glm::mat4 projectionWater = glm::perspective(
		glm::radians(45.0f),
		(float)retina_width / (float)retina_height,
		0.1f,
		1000.0f
	);
	GLint projectionLoc = glGetUniformLocation(waterShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionWater));

	// 2. Desenează mesh-ul de apă
	myWater.Draw(waterShader);
}

void renderScene() {
    // PASS 1: Creare shadow map
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    depthShader.useShaderProgram();
    glm::mat4 lightSpaceMatrix = computeLightSpaceTrMatrix();
    GLuint depthLightSpaceLoc = glGetUniformLocation(depthShader.shaderProgram, "lightSpaceMatrix");
    glUniformMatrix4fv(depthLightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    drawObjects(depthShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);

		return;  // IMPORTANT: ieșire din funcție -> nu mai randăm pass-ul final
	}
    // PASS 2: Randare finală a scenei cu umbre
    glViewport(0, 0, retina_width, retina_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myBasicShader.useShaderProgram();
    // Transmitem lightSpaceMatrix la shader-ul principal
    GLuint mainLightSpaceLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix");
    glUniformMatrix4fv(mainLightSpaceLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Legăm textura de adâncime la unitatea 3 și o trimitem uniformului
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

    // Randăm skybox-ul
    mySkyBox.Draw(skyboxShader, view, projection);

    // Randăm obiectele cu umbre
    drawObjects(myBasicShader, false);

    // Randăm obiectele suplimentare (ex. apă) dacă ai
    renderWater();

}
void cleanup() {
	glfwDestroyWindow(glWindow);
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initOpenGLState();
	initObjects();
	initSkybox();
	initShaders();
	initUniforms();
	initWaterUniforms();
	initFBO();
	fogDensity = 0.0f;

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(glWindow)) {

		processMovement();
		updateSpotlightDirection();  // Actualizează direcția spotlightului
		renderScene();

		if (pressedKeys[GLFW_KEY_L] || pressedKeys[GLFW_KEY_F] || pressedKeys[GLFW_KEY_H] || pressedKeys[GLFW_KEY_G] || pressedKeys[GLFW_KEY_N] || pressedKeys[GLFW_KEY_M])
		{
			renderScene();
			initUniforms();
			initSkybox();
			initWaterUniforms();  // <--- adăugat
		}

		glfwPollEvents();
		glfwSwapBuffers(glWindow);

		//glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
}