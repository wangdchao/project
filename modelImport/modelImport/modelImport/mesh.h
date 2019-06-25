#ifndef _MESH_H_
#define _MESH_H_

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>       
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "physics.h"
#include "shader.h"
using namespace std;

// ��ʾһ����������
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

// ��������
enum ETextureType
{
	ETextureTypeDiffuse = 1,  // ������
	ETextureTypeSpecular = 2, // ���淴��
};

// ��ʾһ��Texture
struct Texture
{
	GLuint id;
	aiTextureType type;
	std::string path;
};


// ��ʾһ��������Ⱦ����Сʵ��
class Mesh
{
public:
	bool flag = false;
	void draw(Shader& shader, glm::vec3 color) // ����Mesh
	{
		shader.Use();
		shader.setVec3("object_Color", color);
		glBindVertexArray(this->VAOId);
		int diffuseCnt = 0, specularCnt = 0,texUnitCnt = 0;
		for (std::vector<Texture>::const_iterator it = this->textures.begin(); 
			this->textures.end() != it; ++it)
		{
			switch (it->type)
			{
				case aiTextureType_DIFFUSE:
				{
					glActiveTexture(GL_TEXTURE0 + texUnitCnt);
					glBindTexture(GL_TEXTURE_2D, it->id);
					std::stringstream samplerNameStr;
					samplerNameStr << "texture_diffuse" << diffuseCnt++;
					glUniform1i(glGetUniformLocation(shader.ID, 
						samplerNameStr.str().c_str()), texUnitCnt++);
				}
				break;
				case aiTextureType_SPECULAR:
				{
						glActiveTexture(GL_TEXTURE0 + texUnitCnt);
						glBindTexture(GL_TEXTURE_2D, it->id);
						std::stringstream samplerNameStr;
						samplerNameStr << "texture_specular" << specularCnt++;
						glUniform1i(glGetUniformLocation(shader.ID,
									samplerNameStr.str().c_str()), texUnitCnt++);
				}
				break;
				default:
					std::cerr << "Warning::Mesh::draw, texture type" << it->type 
						<< " current not supported." << std::endl;
					break;
			}
		}
		
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	Mesh(){}
	Mesh(const std::vector<Vertex>& vertData, 
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices) // ����һ��Mesh
	{
		setData(vertData, textures, indices);
	}
	void setData(const std::vector<Vertex>& vertData,
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices)
	{
		this->vertData = vertData;
		this->indices = indices;
		this->textures = textures;
		this->setupMesh();
	}
	bool explode(Shader& shader) {
		time += 0.5;
		if (time > 1.5f) {
			return true;
		}
		shader.SetFloat("time", time, true);
		return false;
	}
	void final() const
	{
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
		glDeleteBuffers(1, &this->EBOId);
	}
	Circle get_circle(float height) {
		vector<glm::vec2> points;
		float delta = 0.06955;
		for (auto & v : this->vertData) {
			if (v.position.y >= height-delta && v.position.y <= height) {
				points.push_back(glm::vec2(v.position.x, v.position.z));
				if (points.size() == 10) {
					break;
				}
			}
		}
		Circle c = Calculate_circle(points[0], points[5], points[9]);
		//cout << c.x << " " << c.y << " " << c.r << endl;
		return c;
	}
	float get_height() {
		float min = 1000.0f;
		float max = 0.0f;
		for (auto & v : this->vertData) {
			if (min > v.position.y) {
				min = v.position.y;
			}
			if (max < v.position.y) {
				max = v.position.y;
			}
		}
		float center = (max + min) / 2;
		return center;
	}

	~Mesh()
	{
		// ��Ҫ�������ͷ�VBO�ȿռ� ��ΪMesh���󴫵�ʱ ��ʱ�������ٺ����������VBO�ȿռ�
	}
private:
	std::vector<Vertex> vertData;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	GLuint VAOId, VBOId, EBOId;
	float time = -0.0f;

	void setupMesh()  // ����VAO,VBO�Ȼ�����
	{
		//for (auto v : this->vertData) {
			//cout << v.position.x << endl;
		//}
		glGenVertexArrays(1, &this->VAOId);
		glGenBuffers(1, &this->VBOId);
		glGenBuffers(1, &this->EBOId);

		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertData.size(),
			&this->vertData[0], GL_STATIC_DRAW);
		// ����λ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// ������������
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		// ���㷨��������
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		// ��������
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* this->indices.size(),
			&this->indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

#endif 