// ����GLEW�� ���徲̬����
#define GLEW_STATIC
#include <GLEW/glew.h>
// ����GLFW��
#include <GLFW/glfw3.h>
// ����SOIL��
#include <SOIL/SOIL.h>
// ����GLM��
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>

// ������ɫ�����ؿ�
#include "ResourceManager.h"
// ����������Ƹ�����
#include "camera.h"
// �������������
#include "texture.h"
// ����ģ�͵���
#include "model.h"


// ���̻ص�����ԭ������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// ����ƶ��ص�����ԭ������
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// �����ֻص�����ԭ������
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// �������ƶ�
void do_movement();

// ���������
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// ���������������
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // ���������¼
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��

GLfloat angle = 0.0, fontangle = 0.0, rightangle = 0.0;
glm::vec3 coord = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 offset = glm::vec3(0.213184*0.02,1.319366*0.02, 20.650927*0.02);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

enum ViewPort viewfrom = FREE;

Model objModel;

int main(int argc, char** argv)
{

	if (!glfwInit())	// ��ʼ��glfw��
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// ����OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// ��������
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of loading model with AssImp with light on", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	// �����Ĵ��ڵ�contextָ��Ϊ��ǰcontext
	glfwMakeContextCurrent(window);

	// ע�ᴰ�ڼ����¼��ص�����
	glfwSetKeyCallback(window, key_callback);
	// ע������¼��ص�����
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// ע���������¼��ص�����
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// ��겶�� ͣ���ڳ�����
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ��ʼ��GLEW ��ȡOpenGL����
	glewExperimental = GL_TRUE; // ��glew��ȡ������չ����
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// �����ӿڲ���
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Section1 ����ģ������
	if (!objModel.loadModel("../../../resources/models/baolingqiu/BowlingSetup.obj"))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	
	// Section2 ׼����ɫ������
	ResourceManager::LoadShader("model.vertex", "model.frag", nullptr, "objModel",true);
	ResourceManager::LoadShader("model.vertex", "model.frag", nullptr, "ballModel",true);
	ResourceManager::LoadShader("model.vertex", "model.frag", "model.geometry", "pinModel", true);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	
	// ��ʼ��Ϸ��ѭ��
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // ����������� ���̵��¼�
		do_movement(); // �����û�������� �����������

					   // �����ɫ������ ����Ϊָ����ɫ
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// ���ù۲���λ��
		if (viewfrom == FREE) {
			ResourceManager::GetShader("objModel").SetVector3f("viewPos", camera.position.x, camera.position.y, camera.position.z, true);
			ResourceManager::GetShader("ballModel").SetVector3f("viewPos", camera.position.x, camera.position.y, camera.position.z, true);
			ResourceManager::GetShader("pinModel").SetVector3f("viewPos", camera.position.x, camera.position.y, camera.position.z, true);
		}
		else if (viewfrom == BOWLING) {
			ResourceManager::GetShader("objModel").SetVector3f("viewPos", camera.bowlingposition.x, camera.bowlingposition.y, camera.bowlingposition.z, true);
			ResourceManager::GetShader("ballModel").SetVector3f("viewPos", camera.bowlingposition.x, camera.bowlingposition.y, camera.bowlingposition.z, true);
			ResourceManager::GetShader("pinModel").SetVector3f("viewPos", camera.bowlingposition.x, camera.bowlingposition.y, camera.bowlingposition.z, true);
		}
		glm::mat4 projection;
		if(viewfrom==FREE) projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.001f, 1000.0f); // ͶӰ����
		else if(viewfrom==BOWLING) projection = glm::perspective(30.0f,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.001f, 100.0f);
		ResourceManager::GetShader("objModel").SetMatrix4("projection", projection, true);
		ResourceManager::GetShader("ballModel").SetMatrix4("projection", projection, true);
		ResourceManager::GetShader("pinModel").SetMatrix4("projection", projection, true);
		glm::mat4 view = viewfrom==FREE?camera.getViewMatrix():camera.getViewMatrixB(); // �ӱ任����
		ResourceManager::GetShader("objModel").SetMatrix4("view", view, true);
		ResourceManager::GetShader("ballModel").SetMatrix4("view", view, true);
		ResourceManager::GetShader("pinModel").SetMatrix4("view", view, true);
		glm::mat4 objmodel=glm::mat4(1.0f),ballmodel=glm::mat4(1.0f);
		objmodel = glm::translate(objmodel, glm::vec3(0.0f, -1.55f, 0.0f)); // �ʵ��µ�λ��
		objmodel = glm::scale(objmodel, glm::vec3(0.02f, 0.02f, 0.02f)); // �ʵ���Сģ��
		ResourceManager::GetShader("objModel").SetMatrix4("model", objmodel, true);
		ResourceManager::GetShader("pinModel").SetMatrix4("model", objmodel, true);
		ballmodel = glm::translate(ballmodel, glm::vec3(0.0f, -1.55f,0.0f) + offset);
		ballmodel = glm::rotate(ballmodel, glm::radians(angle), coord);
		ballmodel = glm::translate(ballmodel, glm::vec3(-0.213184*0.02, -1.319366*0.02, -20.650927*0.02));
		ballmodel= glm::scale(ballmodel, glm::vec3(0.02f, 0.02f, 0.02f));
		ResourceManager::GetShader("ballModel").SetMatrix4("model", ballmodel, true);
		camera.bowlingposition = glm::vec3(0.0f, -1.45f, 0.0f) + offset;
		camera.bowlingup = glm::vec3(0.0f, -1.0f, 0.0f);
		camera.bowlingforward = glm::vec3(0.0, -1.0, -1.0);
		// ������д�������ƴ���
		objModel.draw(ResourceManager::GetShader("objModel"),ResourceManager::GetShader("ballModel"), ResourceManager::GetShader("pinModel")); // ��������
		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // ��������
	}
	// �ͷ���Դ
	glfwTerminate();
	return 0;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true;
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (viewfrom==FREE) {
		if (firstMouseMove) // �״�����ƶ�
		{
			lastX = xpos;
			lastY = ypos;
			firstMouseMove = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.handleMouseMove(xoffset, yoffset);
	}
}
// ����������ദ�������ֿ���
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(viewfrom == FREE) camera.handleMouseScroll(yoffset);
}
// ����������ദ����̿���
void do_movement()
{
	if (keyPressedStatus[GLFW_KEY_P]) {
		viewfrom = BOWLING;
	}
	if (keyPressedStatus[GLFW_KEY_L]) {
		viewfrom = FREE;
	}
	if (keyPressedStatus[GLFW_KEY_O]) {
		viewfrom = HUMAN;
	}
	if (keyPressedStatus[GLFW_KEY_W] && viewfrom == FREE)
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S] && viewfrom == FREE)
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A] && viewfrom == FREE)
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D] && viewfrom == FREE)
		camera.handleKeyPress(RIGHT, deltaTime);
	//����С����ƶ�
	if (keyPressedStatus[GLFW_KEY_H])
	{
		float dis=objModel.move_ball(0);
		offset += glm::vec3(dis*0.02, 0.0f, 0.0f);
		rightangle -= dis == 0.0 ? 0 : 20*fabs(dis/0.05);
		angle = rightangle;
		coord = glm::vec3(0.0, 0.0, 1.0);
		//camera.bowlingforward = glm::vec3(-1.0, -1.0, 0.0);
	}
	if (keyPressedStatus[GLFW_KEY_K])
	{
		float dis=objModel.move_ball(1);
		offset += glm::vec3(0.02*dis, 0.0f, 0.0f);
		rightangle += dis == 0.0 ? 0 : 20* fabs(dis / 0.05);
		angle = rightangle;
		coord = glm::vec3(0.0, 0.0, 1.0);
		//camera.bowlingforward = glm::vec3(1.0, -1.0, 0.0);
	}
	if (keyPressedStatus[GLFW_KEY_U])
	{
		float dis=objModel.move_ball(2);
		offset += glm::vec3(0.0f, 0.0f, 0.02f*dis);
		fontangle += dis == 0.0 ? 0 : 20* fabs(dis / 0.05);
		angle = fontangle;
		coord = glm::vec3(1.0, 0.0, 0.0);
		
	}
	if (keyPressedStatus[GLFW_KEY_J])
	{
		float dis=objModel.move_ball(3);
		offset += glm::vec3(0.0f, 0.0f, 0.02*dis);
		fontangle -= dis==0.0?0:20* fabs(dis / 0.05);
		angle = fontangle;
		coord = glm::vec3(1.0, 0.0, 0.0);
		//camera.bowlingforward = glm::vec3(0.0, -1.0, 1.0);
	}
}