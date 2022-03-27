#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <string>
#include "Particle.h"
#include<glm.hpp>
#include <gtc/matrix_transform.hpp>
#include"Material.h"
#include"DirectionalLight.h"
#include"LightPoint.h"
#include"LightSpot.h"
#include"Mesh.h"
#include"Model.h"
#include "Skybox.h"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLfloat screenWidth = 800;
GLfloat screenHeight = 600;
Camera::Camera camera(glm::vec3(10.0f, 10.0f, 20.0f));
bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
//string str_fps;
//char c[8];
//int FrameRate = 0;
//int FrameCount = 0;
//int timeLeft = 0;
#pragma region  Light Declare 
DirectionalLight lightD(glm::vec3(5.0f, 10.0f, 20.0f),
	glm::vec3(glm::radians(90.0f), glm::radians(0.0f), 0),
	glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightP0(glm::vec3(5.0f, 10.0f, 20.0f), //四个点光源
	glm::vec3(glm::radians(0.0f), glm::radians(0.0f), 0),
	glm::vec3(1.0f, 1.0f, 1.0f));
LightPoint lightP1(glm::vec3(5.0f, 0.0f, 0.0f),
	glm::vec3(glm::radians(0.0f), glm::radians(0.0f), 0),
	glm::vec3(3.0f, 0, 0));
LightPoint lightP2(glm::vec3(0, 8.0f, 0),
	glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0),
	glm::vec3(0, 3.0f, 0));
LightPoint lightP3(glm::vec3(0, 0, 8.0f),
	glm::vec3(glm::radians(45.0f), glm::radians(45.0f), 0),
	glm::vec3(0, 0, 3.0f));
LightSpot lightS(glm::vec3(1.0f, 1.0f, 10.0f),  //一个聚光源
	glm::vec3(glm::radians(0.0f), glm::radians(90.0f), 0),
	glm::vec3(10.0f, 10.0f, 10.0f));
#pragma endregion

#pragma region LoadImageDataToGPU
unsigned int LoadImageDataToGPU(const char* filename, GLint internalFormat, GLenum format, int textureSlot) {
	unsigned int TexBuffer;
	glGenTextures(1, &TexBuffer);
	glActiveTexture(GL_TEXTURE0 + textureSlot);//栏位
	glBindTexture(GL_TEXTURE_2D, TexBuffer);//绑上接口

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return TexBuffer;
}
#pragma endregion

int main(int argc, char* argv[]) {
	string exePath = argv[0];
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Learn OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}
	glViewport(0, 0, screenWidth, screenHeight);
	Shader* skyShader = new Shader("sky.vert", "sky.frag");
	Shader* meShader = new Shader("light.vert", "light.frag");
#pragma region Init Material
	//unsigned int cubeTexture = LoadImageDataToGPU("container2.png", GL_RGBA, GL_RGBA, Shader::DIFFUSE);

	Material* myMaterial = new Material(meShader,
		//glm::vec3(0,0, 1.0f),//diffuse
		LoadImageDataToGPU("flower3.jpg", GL_RGBA, GL_RGBA, Shader::DIFFUSE),
		LoadImageDataToGPU("flower.jpg", GL_RGBA, GL_RGBA, Shader::SPECULAR),
		LoadImageDataToGPU("flower.jpg", GL_RGB, GL_RGB, Shader::EMISSION),
		//glm::vec3 (1.0f, 1.0f, 1.0f),//specular
		glm::vec3(1.0f, 1.0f, 1.0f),//ambient
		32.0f);//shininess
	Model loadmodel2(exePath.substr(0, exePath.find_last_of('\\')) + "\\model\\flo\\flo.obj");
	Skybox skybox(0);
	skyShader->setInt("skybox", 0);
	glm::mat4 model;
	//glm::mat4 model2;
	glm::mat4 view;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);

	Particle::Particle floParticle;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		do_movement();
		//render
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 flowprojection(1.0f);
		glm::mat4 flowmodel(1.0f);
		glm::mat4 flowview = camera.GetViewMatrix();
		flowprojection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 2000.f);
		floParticle.Render(deltaTime, flowmodel, flowview, flowprojection);

		view = camera.GetViewMatrix();	 //！viewMatrix要放置在渲染循环中，才会有鼠标转动
		meShader->use();
		model = glm::scale(glm::mat4(1.0f), glm::vec3(0.3, 0.3, 0.3));
		float angle = 0.1f;
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0))); // rotate the quad to show normal mapping from multiple directions
#pragma region SetUniform
		glUniformMatrix4fv(glGetUniformLocation(meShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(meShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(meShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(glGetUniformLocation(meShader->ID, "objColor"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(meShader->ID, "ambientColor"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightPos"), 1.0f + sin(glfwGetTime()) * 2.0f, sin(glfwGetTime() / 2.0f) * 1.0f, -1.0f);
		glUniform3f(glGetUniformLocation(meShader->ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
#pragma region 设置灯光
		glUniform3f(glGetUniformLocation(meShader->ID, "lightD.pos"), lightD.position.x, lightD.position.y, lightD.position.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightD.dirToLight"), lightD.direction.x, lightD.direction.y, lightD.direction.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightD.color"), lightD.color.x, lightD.color.y, lightD.color.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP0.pos"), lightP0.position.x, lightP0.position.y, lightP0.position.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP0.dirToLight"), lightP0.direction.x, lightP0.direction.y, lightP0.direction.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP0.color"), lightP0.color.x, lightP0.color.y, lightP0.color.z);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP0.constant"), lightP0.constant);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP0.linear"), lightP0.linear);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP0.quadratic"), lightP0.quadratic);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP1.pos"), lightP1.position.x, lightP1.position.y, lightP1.position.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP1.dirToLight"), lightP1.direction.x, lightP1.direction.y, lightP1.direction.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP1.color"), lightP1.color.x, lightP1.color.y, lightP1.color.z);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP1.constant"), lightP1.constant);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP1.linear"), lightP1.linear);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP1.quadratic"), lightP1.quadratic);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP2.pos"), lightP2.position.x, lightP2.position.y, lightP2.position.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP2.dirToLight"), lightP2.direction.x, lightP2.direction.y, lightP2.direction.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP2.color"), lightP2.color.x, lightP2.color.y, lightP2.color.z);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP2.constant"), lightP2.constant);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP2.linear"), lightP2.linear);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP2.quadratic"), lightP2.quadratic);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP3.pos"), lightP3.position.x, lightP3.position.y, lightP3.position.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP3.dirToLight"), lightP3.direction.x, lightP3.direction.y, lightP3.direction.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightP3.color"), lightP3.color.x, lightP3.color.y, lightP3.color.z);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP3.constant"), lightP3.constant);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP3.linear"), lightP3.linear);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightP3.quadratic"), lightP3.quadratic);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightS.pos"), lightS.position.x, lightS.position.y, lightS.position.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightS.dirToLight"), lightS.direction.x, lightS.direction.y, lightS.direction.z);
		glUniform3f(glGetUniformLocation(meShader->ID, "lightS.color"), lightS.color.x, lightS.color.y, lightS.color.z);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightS.constant"), lightS.constant);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightS.linear"), lightS.linear);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightS.quadratic"), lightS.quadratic);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightS.cosPhyInner"), lightS.cosPhyInner);
		glUniform1f(glGetUniformLocation(meShader->ID, "lightS.cosPhyOutter"), lightS.cosPhyOutter);
#pragma endregion
		//设置材质
		myMaterial->shader->SetUniform3f("material.ambinet", myMaterial->ambient);
		myMaterial->shader->SetUniform1i("material.diffuse", Shader::DIFFUSE); //0号栏位灌入diffuse素材sampler2D图
		myMaterial->shader->SetUniform1i("material.specular", Shader::SPECULAR);  //1号栏位灌入specular素材sampler2D图
		myMaterial->shader->SetUniform1i("material.emission", Shader::EMISSION);
		myMaterial->shader->SetUniform1f("material.shininess", myMaterial->shininess);


#pragma endregion
		loadmodel2.Draw(myMaterial->shader);
		//glDepthFunc(GL_LEQUAL);
		//skyShader->use();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		//glUniformMatrix4fv(glGetUniformLocation(skyShader->ID, "skyView"), 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(glGetUniformLocation(skyShader->ID, "skyProj"), 1, GL_FALSE, glm::value_ptr(projection));
		//skybox.Draw();
		//glDepthFunc(GL_LESS); // set depth function back to default

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void do_movement() {
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(Camera::LEFTS, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(Camera::RIGHTS, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
