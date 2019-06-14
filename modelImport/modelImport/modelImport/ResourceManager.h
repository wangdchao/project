#pragma once
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <map>
#include "shader.h"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public:
	// Resource storage
	static std::map<std::string, Shader>    Shaders;
	//static std::map<std::string, Texture2D> Textures;
	// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
	static Shader   LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name,bool set);
	// Retrieves a stored sader
	static Shader &  GetShader(std::string name);
	static void      Clear();

	//设定shader共性值
	static void setCommonValue(Shader &shader);
private:
	// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
	ResourceManager() { }
	// Loads and generates a shader from file
	static Shader    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
};

#endif