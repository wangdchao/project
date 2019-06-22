// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GLEW/glew.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
// 引入SOIL库
#include <SOIL/SOIL.h>
// 引入GLM库
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>

// 包含着色器加载库
#include "ResourceManager.h"
// 包含相机控制辅助类
#include "camera.h"
// 包含纹理加载类
#include "texture.h"
// 加载模型的类
#include "model.h"


// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// 鼠标移动回调函数原型声明
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// 鼠标滚轮回调函数原型声明
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 场景中移动
void do_movement();

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// 用于相机交互参数
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // 按键情况记录
GLfloat deltaTime = 0.0f; // 当前帧和上一帧的时间差
GLfloat lastFrame = 0.0f; // 上一帧时间

GLfloat angle = 0.0, fontangle = 0.0, rightangle = 0.0;
glm::vec3 coord = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 offset = glm::vec3(0.213184*0.02,1.319366*0.02, 20.650927*0.02);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

enum ViewPort viewfrom = FREE;

Model objModel;

int main(int argc, char** argv)
{

	if (!glfwInit())	// 初始化glfw库
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// 开启OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of loading model with AssImp with light on", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);

	// 注册窗口键盘事件回调函数
	glfwSetKeyCallback(window, key_callback);
	// 注册鼠标事件回调函数
	glfwSetCursorPosCallback(window, mouse_move_callback);
	// 注册鼠标滚轮事件回调函数
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// 鼠标捕获 停留在程序内
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化GLEW 获取OpenGL函数
	glewExperimental = GL_TRUE; // 让glew获取所有拓展函数
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Section1 加载模型数据
	if (!objModel.loadModel("../../../resources/models/baolingqiu/BowlingSetup.obj"))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}
	
	// Section2 准备着色器程序
	ResourceManager::LoadShader("model.vertex", "model.frag", nullptr, "objModel",true);
	ResourceManager::LoadShader("model.vertex", "model.frag", nullptr, "ballModel",true);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		do_movement(); // 根据用户操作情况 更新相机属性

					   // 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// 设置观察者位置
		if (viewfrom == FREE) {
			ResourceManager::GetShader("objModel").SetVector3f("viewPos", camera.position.x, camera.position.y, camera.position.z, true);
			ResourceManager::GetShader("ballModel").SetVector3f("viewPos", camera.position.x, camera.position.y, camera.position.z, true);
		}
		else if (viewfrom == BOWLING) {
			ResourceManager::GetShader("objModel").SetVector3f("viewPos", camera.bowlingposition.x, camera.bowlingposition.y, camera.bowlingposition.z, true);
			ResourceManager::GetShader("ballModel").SetVector3f("viewPos", camera.bowlingposition.x, camera.bowlingposition.y, camera.bowlingposition.z, true);
		}
		glm::mat4 projection;
		if(viewfrom==FREE) projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.001f, 1000.0f); // 投影矩阵
		else if(viewfrom==BOWLING) projection = glm::perspective(30.0f,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.001f, 100.0f);
		ResourceManager::GetShader("objModel").SetMatrix4("projection", projection, true);
		ResourceManager::GetShader("ballModel").SetMatrix4("projection", projection, true);

		glm::mat4 view = viewfrom==FREE?camera.getViewMatrix():camera.getViewMatrixB(); // 视变换矩阵
		ResourceManager::GetShader("objModel").SetMatrix4("view", view, true);
		ResourceManager::GetShader("ballModel").SetMatrix4("view", view, true);

		glm::mat4 objmodel=glm::mat4(1.0f),ballmodel=glm::mat4(1.0f);
		objmodel = glm::translate(objmodel, glm::vec3(0.0f, -1.55f, 0.0f)); // 适当下调位置
		objmodel = glm::scale(objmodel, glm::vec3(0.02f, 0.02f, 0.02f)); // 适当缩小模型
		ResourceManager::GetShader("objModel").SetMatrix4("model", objmodel, true);

		ballmodel = glm::translate(ballmodel, glm::vec3(0.0f, -1.55f,0.0f) + offset);
		ballmodel = glm::rotate(ballmodel, glm::radians(angle), coord);
		ballmodel = glm::translate(ballmodel, glm::vec3(-0.213184*0.02, -1.319366*0.02, -20.650927*0.02));
		ballmodel= glm::scale(ballmodel, glm::vec3(0.02f, 0.02f, 0.02f));
		ResourceManager::GetShader("ballModel").SetMatrix4("model", ballmodel, true);
		camera.bowlingposition = glm::vec3(0.0f, -1.45f, 0.0f) + offset;
		camera.bowlingup = glm::vec3(0.0f, -1.0f, 0.0f);
		camera.bowlingforward = glm::vec3(0.0, -1.0, -1.0);
		// 这里填写场景绘制代码
		objModel.draw(ResourceManager::GetShader("objModel"),ResourceManager::GetShader("ballModel")); // 绘制物体
		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
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
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (viewfrom==FREE) {
		if (firstMouseMove) // 首次鼠标移动
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
// 由相机辅助类处理鼠标滚轮控制
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if(viewfrom == FREE) camera.handleMouseScroll(yoffset);
}
// 由相机辅助类处理键盘控制
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
	//控制小球的移动
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