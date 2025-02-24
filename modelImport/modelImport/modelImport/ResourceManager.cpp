#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

//光照位置
glm::vec3 lampPos(0.0f, 0.5f, 0.7f);
// Instantiate static variables
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name,bool set)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	if(set) setCommonValue(Shaders[name]);
	return Shaders[name];
}

Shader & ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}


void ResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();
	// 2. Now create shader object from source code
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

void ResourceManager::setCommonValue(Shader &shader) {
	shader.Use();
	// 设置光源属性 点光源
	GLint lightAmbientLoc = glGetUniformLocation(shader.ID, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(shader.ID, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(shader.ID, "light.specular");
	GLint lightPosLoc = glGetUniformLocation(shader.ID, "light.position");
	GLint attConstant = glGetUniformLocation(shader.ID, "light.constant");
	GLint attLinear = glGetUniformLocation(shader.ID, "light.linear");
	GLint attQuadratic = glGetUniformLocation(shader.ID, "light.quadratic");

	glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	glUniform3f(lightDiffuseLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
	// 设置衰减系数
	glUniform1f(attConstant, 1.0f);
	glUniform1f(attLinear, 0.09f);
	glUniform1f(attQuadratic, 0.032f);
}