// GLMTest].cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>

#include "Thirdparty/glm/glm/glm.hpp"
#include "Thirdparty/glm/glm/gtc/matrix_transform.hpp"
#include "Thirdparty/glm/glm/gtc/type_ptr.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "Thirdparty/stb_image.h"

#ifndef GLSL
#define GLSL(version, A) "#version " #version "\n" #A
#endif

using namespace glm;

const char * vertexShaderCode = GLSL(120,

	attribute vec4 position;
attribute vec4 color;
attribute vec2 textureCoor;
attribute vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec4 dstColor;
varying vec2 fTextureCoor; //<- pass texture to fragment shader

void main()
{
	// Todo: transform normal vector to world space
	// by multiply with $normalMat = transpose(inverse(viewMat*modelMat)) in CPU
	// vec3 nNormal = normalize(normalMat*normal);
	// vvvv This normal is in local space, not in world space yet!
	vec3 nNormal = normalize(normal);

	// Todo: move light position to CPU, so we can manipulate it.
	vec3 lightPos = normalize(vec3(10, 10, 10));

	float lightIntensity = max(dot(nNormal, lightPos), 0.0f);

	fTextureCoor = textureCoor; //<- pass texture to fragment shader
	// color * defuse light intensity
	dstColor = vec4((color.rgb * lightIntensity), 1.0);
	gl_Position = projection * view*model*position;
}

);

const char * fragmentShaderCode = GLSL(120,

	uniform sampler2D texture; //<- texture data

varying vec2 fTextureCoor; //<- texture coor from vertices
varying vec4 dstColor;

void main()
{

	float ambientStrength = 0.4f;
	vec4 ambientLight = vec4(1.0, 1.0, 1.0, 1.0) * ambientStrength;
	vec4 material = vec4(1.0, 1.0, 1.0, 1.0);

	// TODO: move defuse light to fragment shader
	// TODO: add specular light

	gl_FragColor = texture2D(texture, fTextureCoor) * (dstColor + ambientLight) * material;
}

);


/*-----------------------------------------------------------------------------
*  FUNCION TO CHECK FOR SHADER COMPILER ERRORS
*-----------------------------------------------------------------------------*/
void compilerCheck(GLuint ID) {
	GLint comp;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &comp);

	if (comp == GL_FALSE) {
		std::cout << "Shader Compilation FAILED" << std::endl;
		GLchar messages[256];
		glGetShaderInfoLog(ID, sizeof(messages), 0, &messages[0]);
		std::cout << messages;
	}
}

void linkCheck(GLuint ID) {
	GLint linkStatus, validateStatus;
	glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE) {
		std::cout << "Shader Linking FAILED" << std::endl;
		GLchar messages[256];
		glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
		std::cout << messages;
	}

	glValidateProgram(ID);
	glGetProgramiv(ID, GL_VALIDATE_STATUS, &validateStatus);

	std::cout << "Link: " << linkStatus << "  Validate: " << validateStatus << std::endl;
	if (linkStatus == GL_FALSE) {
		std::cout << "Shader Validation FAILED" << std::endl;
		GLchar messages[256];
		glGetProgramInfoLog(ID, sizeof(messages), 0, &messages[0]);
		std::cout << messages;
	}
}

/*----------------------------------------------------------------
*  Start Main Function
*-----------------------------------------------------------------*/
#include <vector>

struct Vertex
{
	Vertex() = default;

	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoor;
	glm::vec3 normal;
};

std::vector<Vertex> model;
std::vector<GLuint> modelIndices;

void initModel()
{

	//Specify the 8 VERTICES of A Cube
	Vertex cube[] = {
	  {glm::vec3(1, -1,  1), glm::vec4(1,1,1,1), glm::vec2(0.25f, 0.34f), glm::vec3(1, -1,  1)},
	  {glm::vec3(1,  1,  1), glm::vec4(1,1,1,1), glm::vec2(0.25f, 0), glm::vec3(1,  1,  1)},
	  {glm::vec3(-1,  1,  1), glm::vec4(1,1,1,1), glm::vec2(0, 0), glm::vec3(-1,  1,  1)},
	  {glm::vec3(-1, -1,  1), glm::vec4(1,1,1,1), glm::vec2(0, 0.34f), glm::vec3(-1, -1,  1)},

	  {glm::vec3(1, -1, -1), glm::vec4(1,1,1,1), glm::vec2(0.25f, 0.66f), glm::vec3(1, -1, -1)},
	  {glm::vec3(1,  1, -1), glm::vec4(1,1,1,1), glm::vec2(0.25f, 1.0f), glm::vec3(1,  1, -1)},
      {glm::vec3(-1,  1, -1), glm::vec4(1,1,1,1), glm::vec2(0, 1.0f), glm::vec3(-1,  1, -1)},
      {glm::vec3(-1, -1, -1), glm::vec4(1,1,1,1), glm::vec2(0, 0.66f), glm::vec3(-1, -1, -1)},

	  {glm::vec3(1,  1,  1), glm::vec4(1,1,1,1), glm::vec2(0.5f, 0.34f), glm::vec3(1,  1,  1)},
	  {glm::vec3(1,  1, -1), glm::vec4(1,1,1,1), glm::vec2(0.5f, 0.66f), glm::vec3(1,  1, -1)},
	  {glm::vec3(-1,  1, -1), glm::vec4(1,1,1,1), glm::vec2(0.75f, 0.66f), glm::vec3(-1,  1, -1)},
	  {glm::vec3(-1,  1,  1), glm::vec4(1,1,1,1), glm::vec2(0.75f, 0.34f), glm::vec3(-1,  1,  1)},

	  {glm::vec3(-1, -1,  1), glm::vec4(1,1,1,1), glm::vec2(1.0f, 0.34f), glm::vec3(-1, -1,  1)},
	  {glm::vec3(-1, -1, -1), glm::vec4(1,1,1,1), glm::vec2(1.0f, 0.66f), glm::vec3(-1, -1, -1)}
	};

	model.assign(std::begin(cube), std::end(cube));

	GLubyte indices[24] = {
					    0,1,2,3,  //front
					    7,6,5,4, //back
						12,11,10,13, //left
						4,9,8,0, //right
						8,9,10,11, //top
						4,0,3,7 }; //bottom

	modelIndices.assign(std::begin(indices), std::end(indices));
}

// shader pointer
GLuint sID;

// vertex pointers
GLuint positionID;
GLuint colorID;
GLuint texCoorID;

// buffer pointers
GLuint arrayID;
GLuint elementID;
GLuint textureID;
GLuint normalID;

// uniform pointers
GLuint modelID;
GLuint viewID;
GLuint projectionID;

void initShaders()
{
	//open gl create a shader program
	sID = glCreateProgram();
	GLuint vID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fID = glCreateShader(GL_FRAGMENT_SHADER);

	// Load shader source code
	glShaderSource(vID, 1, &vertexShaderCode, nullptr);
	glShaderSource(fID, 1, &fragmentShaderCode, nullptr);

	// Compile
	glCompileShader(vID);
	glCompileShader(fID);

	compilerCheck(vID);
	compilerCheck(fID);

	glAttachShader(sID, vID);
	glAttachShader(sID, fID);

	glLinkProgram(sID);

	linkCheck(sID);

	glUseProgram(sID);

	positionID = glGetAttribLocation(sID, "position");
	colorID = glGetAttribLocation(sID, "color");
	texCoorID = glGetAttribLocation(sID, "textureCoor");
	normalID = glGetAttribLocation(sID, "normal");

	modelID = glGetUniformLocation(sID, "model");
	viewID = glGetUniformLocation(sID, "view");
	projectionID = glGetUniformLocation(sID, "projection");

	glUseProgram(0);
}

void createBuffer()
{
	GLuint bufferID;
	glGenVertexArrays(1, &arrayID);
	glBindVertexArray(arrayID);

	// creat a buffer for vertex array
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	// send the vertex data to GPU memory
	glBufferData(GL_ARRAY_BUFFER, model.size() * sizeof(Vertex), model.data(), GL_STATIC_DRAW);

	// create a buffer of indices
	glGenBuffers(1, &elementID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		modelIndices.size() * sizeof(GLuint),
		modelIndices.data(), GL_STATIC_DRAW);

	// tell a shader program to get access to the data.
	glEnableVertexAttribArray(positionID);
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(texCoorID);
	glVertexAttribPointer(texCoorID, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec4)));
	glEnableVertexAttribArray(normalID);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)(sizeof(glm::vec3)
			+ sizeof(glm::vec4)
			+ sizeof(glm::vec2)));

	// unbind vertex array
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLFWwindow* window;
int screen_width = 1280;
int screen_height = 720;



void initTexture()
{
	/*-----------------------------------------------------------------------------
	 *  Make some rgba data (can also load a file here)
	 *-----------------------------------------------------------------------------*/
	int width, height, nrChannels;
	unsigned char *data = stbi_load("Assets/dice.png",
		&width, &height, &nrChannels, 0);


	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	if (nrChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (nrChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// delete texture data
	stbi_image_free(data);
}

int axisXYZ = 0;
float x, y, z = 0.0f;
static const float Speed = 0.1f;
static const float screenWBorder = 7.0f;
static const float screenHBorder = 4.0f;

void drawArray()
{

	glUseProgram(sID);  // enable the shader program
	glBindTexture(GL_TEXTURE_2D, textureID); // enable the texture

	// get actual window size from GLFW
	glfwGetWindowSize(window, &screen_width, &screen_height);
	// tell opengl to set a new viewport size
	glViewport(0, 0, screen_width, screen_height);

	// move camera position (yo yo effect)
	static float camSpeed = 0.1f;
	static float camPos = 9.0f;
	
	/*
	camPos += camSpeed;
	if (camPos > 9.0f || camPos < 1.0f)
	  camSpeed *= -1;
	*/

	// create a view transformation matrix
	vec3 eyepos = vec3(0.0f, 0.0f, camPos);
	mat4 viewMat = glm::lookAt(eyepos,        //eye position
		vec3(0, 0, 0),  // target
		vec3(0, 1, 0)); // up vector

	float ratio = screen_width / static_cast<float>(screen_height);
	mat4 projectionMat = glm::perspective(1.0f,
		ratio, 0.1f, -10.0f);

	vec3 scaleValue = vec3(1.0f, 1.0f, 1.0f);
	vec3 translationValue = vec3(x, y, z);
	vec3 axis = vec3(1, 0, 0);

	switch (axisXYZ)
	{
	case 0:	axis = vec3(1, 0, 0);
		break;
	case 1:	axis = vec3(0, 1, 0);
		break;
	case 2:	axis = vec3(0, 0, 1);
		break;
	}

	static float radius = 0.0f;
	radius += 0.01f;

	mat4 rotationMat = glm::rotate(mat4(1), radius, axis);
	mat4 translationMat = glm::translate(mat4(1), translationValue);
	mat4 scaleMat = glm::scale(glm::mat4(1), scaleValue);
	mat4 modelMat = translationMat * rotationMat*scaleMat;

	glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(projectionMat));

	glBindVertexArray(arrayID);

	glDrawElements(GL_QUADS, modelIndices.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_W && y < screenHBorder)
		y += Speed;
	if (key == GLFW_KEY_S && y > -screenHBorder)
		y -= Speed;
	if (key == GLFW_KEY_A && x > -screenWBorder)
		x -= Speed;
	if (key == GLFW_KEY_D && x < screenWBorder)
		x += Speed;

	if (key == GLFW_KEY_R)
		axisXYZ = 0;
	if (key == GLFW_KEY_T)
		axisXYZ = 1;
	if (key == GLFW_KEY_Y)
		axisXYZ = 2;
}

int main(void)
{

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screen_width, screen_height, "Dice", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		std::cout << " GLEW is not working. " << std::endl;
		return -1;
	}

	if (!GLEW_VERSION_2_1)
	{
		std::cout << "OpenGL2.1 does not supported." << std::endl;
		return -1;
	}

	const GLubyte *glVersion = glGetString(GL_VERSION);
	std::cout << "Graphics driver: " << glVersion << std::endl;

	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cout << "GLSL version: " << glslVersion << std::endl;

	if (GLEW_ARB_vertex_array_object)
	{
		std::cout << "Vertex arrays is supported" << std::endl;
	}

	/* Set key call back*/
	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_DEPTH_TEST);

	initModel();
	initShaders();
	createBuffer();
	initTexture();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.7f, 0.5f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawArray();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}