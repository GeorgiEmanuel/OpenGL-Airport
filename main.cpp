#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint spotLightPosLoc;
GLint spotLightDirLoc;
GLint spotLightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(-150.0f, 30.0f, 150.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));


GLfloat cameraSpeed = 0.7f;

GLboolean pressedKeys[1024];

// models
gps::Model3D A380;
gps::Model3D A380_2;
gps::Model3D A380_3;
gps::Model3D A310;
gps::Model3D Boeing;
gps::Model3D Lutwaffe;
gps::Model3D Building;
gps::Model3D Runway;
gps::Model3D Park;
gps::Model3D Park_2;
gps::Model3D Grass;
gps::Model3D Park_middle;
gps::Model3D screenQuad;
gps::Model3D lightCube;
gps::Model3D super_hornet;
gps::Model3D super_hornet_2;
gps::Model3D water;
gps::Model3D tree1;
gps::Model3D tree2;



struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

BoundingBox buildingHitbox = {
    glm::vec3(-180.0f, 0.0f, -120.0f), 
    glm::vec3(180.0f, 100.0f, 120.0f)  
};

BoundingBox worldBounds = {
    glm::vec3(-1000.0f, .0f, -1000.0f), 
    glm::vec3(1100.0f, 250.0f, 1100.0f)  
};

GLfloat angle;

//skybox
gps::SkyBox mySkyBox;


// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap = false;

const unsigned int SHADOW_WIDTH = 8048;
const unsigned int SHADOW_HEIGHT = 8048;

glm::vec3 translationVector = glm::vec3(0.0f, 0.0f, 0.0f);
float scaleFactor = 1.0f;
GLfloat lightAngle;


bool polygon_mode = false;

float oldXpos = 640, oldYpos = 400, yaw, pitch, dx, dy, sensitivity = 0.3f;
bool firstMouse = true;


bool isPresentationActive = false;
float presentationTimer = 0.0f;



float start_point = 1000.0f, end_point = -800.0f;
float movement_speed = 100.0;
bool movingForward = true;

bool isNight = false;

double lastTimeStamp = glfwGetTime();

GLenum glCheckError_(const char *file, int line)
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

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        showDepthMap = !showDepthMap;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (key == GLFW_KEY_J && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        isPresentationActive = !isPresentationActive;
        presentationTimer = 0.0f;

    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        isNight = !isNight;
    }

}

void clampAngles() {
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        oldXpos = xpos;
        oldYpos = ypos;
        firstMouse = false;
	}

    dx = xpos - oldXpos;
    dy = oldYpos - ypos; 
    oldXpos = xpos;
    oldYpos = ypos;
    dx *= sensitivity;
    dy *= sensitivity;
    yaw -= dx;
    pitch += dy;
    clampAngles();
    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

void renderPresentation() {
    if (!isPresentationActive) return;

    presentationTimer += 0.01f;

    if (presentationTimer < 4.0f) {
        myCamera.setCameraPosition(
            glm::vec3(0.0f, 350.0f - (presentationTimer * 40.0f), 500.0f),
            glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0), glm::vec3(0, 0, -1)
        );
        myCamera.rotate(-35.0f, 0.0f);
    }
    else if (presentationTimer < 8.0f) {
        float localT = presentationTimer - 4.0f;
        myCamera.setCameraPosition(
            glm::vec3(-700.0f + (localT * 175.0f), 120.0f, 350.0f),
            glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0), glm::vec3(0, 0, -1)
        );
        myCamera.rotate(-20.0f, 45.0f);
    }
    else if (presentationTimer < 12.0f) {
        float localT = presentationTimer - 8.0f;
        myCamera.setCameraPosition(
            glm::vec3(700.0f - (localT * 175.0f), 120.0f, 350.0f),
            glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0), glm::vec3(0, 0, -1)
        );
        myCamera.rotate(-20.0f, -45.0f);
    }
    else {
        myCamera.setCameraPosition(
            glm::vec3(0.0f, 15.0f, 250.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        );
        myCamera.rotate(15.0f, 0.0f);

        isPresentationActive = false;
        presentationTimer = 0.0f;
    }

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

bool isPointInBox(glm::vec3 pos,   BoundingBox box) {
        return (pos.x >= box.min.x && pos.x <= box.max.x) &&
           (pos.y >= box.min.y && pos.y <= box.max.y) &&
			(pos.z >= box.min.z && pos.z <= box.max.z);
}

void processMovement() {

    float cameraRadius = 1.0f;

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);

        if (isPointInBox(myCamera.getPosition(), buildingHitbox) || !isPointInBox(myCamera.getPosition(), worldBounds)) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		}

        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);

        if (isPointInBox(myCamera.getPosition(), buildingHitbox) || !isPointInBox(myCamera.getPosition(), worldBounds)) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        }

        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        if (isPointInBox(myCamera.getPosition(), buildingHitbox) || !isPointInBox(myCamera.getPosition(), worldBounds)) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        }
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        if (isPointInBox(myCamera.getPosition(), buildingHitbox) || !isPointInBox(myCamera.getPosition(), worldBounds)) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        }
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}


}

void initOpenGLWindow() {
    myWindow.Create(1280, 1000, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
	A380.LoadModel("models/A380/A380.obj");
	A380_2.LoadModel("models/A380_2/A380_2.obj");
	A380_3.LoadModel("models/A380_3/A380_3.obj");
	A310.LoadModel("models/A310/A310.obj");
    Boeing.LoadModel("models/Boeing/Boeing.obj");
	Lutwaffe.LoadModel("models/Lutwaffe/Lutwaffe.obj");
    Building.LoadModel("models/Building/Building.obj");
    Runway.LoadModel("models/runway/runway.obj");
    Park.LoadModel("models/park/park.obj");
    Park_2.LoadModel("models/park_2/park_2.obj");
    Grass.LoadModel("models/grass/grass.obj");
    Park_middle.LoadModel("models/park_middle/park_middle.obj");
    lightCube.LoadModel("models/light_cube/cube.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
	super_hornet.LoadModel("models/super_hornet/super_hornet.obj");
    super_hornet_2.LoadModel("models/super_hornet_2/super_hornet_2.obj");
	water.LoadModel("models/water/water.obj");
	tree1.LoadModel("models/tree_1/tree_1.obj");
	tree2.LoadModel("models/tree_2/tree_2.obj");
    
}

void initShaders() {
	myBasicShader.loadShader("shaders/basic.vert","shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depth.vert", "shaders/depth.frag");
	depthMapShader.useShaderProgram();


    
}

void initUniforms() {
	myBasicShader.useShaderProgram();
   
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSource1"), 1, glm::value_ptr(glm::vec3(10.0f, 15.0f, 10.0f)));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSourceColor1"), 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSource2"), 1, glm::value_ptr(glm::vec3(-10.0f, 12.0f, 5.0f)));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSourceColor2"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.4f)));

    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSource3"), 1, glm::value_ptr(glm::vec3(0.0f, 25.0f, -10.0f)));
    glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSourceColor3"), 1, glm::value_ptr(glm::vec3(0.3f, 0.3f, 0.3f)));


    // create model matrix 
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix 
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 1520.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
    lightDir = glm::normalize(glm::vec3(0.0f, 1.0f, -0.3f));
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 0.7f, 0.4f); 
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");

    myBasicShader.useShaderProgram();
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	// send light color to shader
    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    spotLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightPos");
    spotLightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightDir");
    spotLightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "spotLightColor");

}

void initSkyBox() {
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/storm/stormydays_rt.tga");
    faces.push_back("skybox/storm/stormydays_lf.tga");
    faces.push_back("skybox/storm/stormydays_up.tga");
    faces.push_back("skybox/storm/stormydays_dn.tga");
    faces.push_back("skybox/storm/stormydays_bk.tga");
    faces.push_back("skybox/storm/stormydays_ft.tga");
    mySkyBox.Load(faces);
}

void initFBO() {
  
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
        0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete! Status: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {

    glm::vec3 lightPos = glm::vec3(0.0f, 500.0f, 0.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    float near_plane = 1.0f, far_plane = 1000.0f;
    glm::mat4 lightProjection = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, near_plane, far_plane);

    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
            
    return lightSpaceTrMatrix;
}

void updateCoords(double elapsedSeconds) {
    if (start_point > end_point) {
        start_point -= movement_speed * elapsedSeconds;
    }
    else {
		start_point = 1000.0f;
    }
}

void moveFormation(gps::Shader shader)
{
    modelLoc = glGetUniformLocation(shader.shaderProgram, "model");
    double currentTimeStamp = glfwGetTime();
    updateCoords(currentTimeStamp - lastTimeStamp);
    lastTimeStamp = currentTimeStamp;

    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(start_point, 0, 0));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

    
    A380_3.Draw(shader);
    super_hornet.Draw(shader);
    super_hornet_2.Draw(shader);
}

void drawObjects(gps::Shader shader, bool depthPass)
{
    shader.useShaderProgram();

    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"),1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    A380.Draw(shader);
    A310.Draw(shader);
    A380_2.Draw(shader);
    //A380_3.Draw(shader);
    Boeing.Draw(shader);
    Lutwaffe.Draw(shader);
    Building.Draw(shader);
    Runway.Draw(shader);
    Park.Draw(shader);
    Park_2.Draw(shader);
    Grass.Draw(shader);
    Park_middle.Draw(shader);
	water.Draw(shader);
	tree1.Draw(shader);
	tree2.Draw(shader);
	moveFormation(shader);
}

void renderScene() {


    depthMapShader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    drawObjects(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


    if (showDepthMap) {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

     
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)
        myBasicShader.useShaderProgram();
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "nightMode"), isNight ? 1 : 0);

        glm::vec3 spotPos = glm::vec3(120.0f, 45.0f, 120.0f);
        glm::vec3 spotDir = glm::vec3(0.2f, -1.0f, -0.1f);
        glm::vec3 spotCol = isNight ? glm::vec3(5.0f, 5.0f, 5.0f) : glm::vec3(0.0f);

        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightPos"), 1, glm::value_ptr(spotPos));
        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightDir"), 1, glm::value_ptr(spotDir));
        glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "spotLightColor"), 1, glm::value_ptr(spotCol));

        if (isNight) {
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(0.01f, 0.01f, 0.01f)));
            glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        }
        else {
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.7f, 0.4f)));
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        }



        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

       
        glm::vec3 currentLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * currentLightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),1,GL_FALSE,glm::value_ptr(computeLightSpaceTrMatrix()));

        drawObjects(myBasicShader, false);

        //draw a white cube around the light

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        model = lightRotation;
        model = glm::translate(model, 100.0f * lightDir);
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lightCube.Draw(lightShader);

        skyboxShader.useShaderProgram();
        glUniform1f(glGetUniformLocation(skyboxShader.shaderProgram, "nightFactor"), isNight ? 0.03f : 1.0f);
        mySkyBox.Draw(skyboxShader, view, projection);
    }
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
    initFBO();
	initSkyBox();
	initUniforms();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        if (isPresentationActive) {
            renderPresentation();
		}
        else {
            processMovement();
        }

	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
