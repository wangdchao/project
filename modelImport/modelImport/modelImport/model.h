#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "texture.h"
#include "physics.h"
#include <map>
#include <windows.h>
#include <mmsystem.h>
#pragma   comment(lib,"winmm.lib") 
/*
* 代表一个模型 模型可以包含一个或多个Mesh
*/
//是瓶子的mesh的序号：3/5/7/9/11/13/15/17/19/22
//声音管理引擎
class Model
{
public:
	void draw(Shader& shader,Shader &shader1,Shader &shader2)
	{
		//printf("Circle: %f %f %f\n", bcircle.x, bcircle.y, hight);
		int i = 0;
		for (std::vector<Mesh>::iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			if (it->flag) {
				i++;
				continue;
			}
			if (i == 21) {
				it->draw(shader1, glm::vec3(0.572549f, 0.258824f, 0.858824f));
			}
			else if (this->is_pin[i] == 1) {
				shader2.SetFloat("time", -3.1415926535f / 2.0f, true);
				it->draw(shader2, glm::vec3(1.0f, 1.0f, 1.0f));
			}
			else if (i > 1 && this->is_pin[i-1] == 1){
				//cout <<sin(glfwGetTime() / 10.0f) << endl;
				shader2.SetFloat("time", -3.1415926535f/2.0f, true);
				it->draw(shader2, glm::vec3(0.639216f, 0.478431f, 0.807843f));
			}
			else {
				it->draw(shader, glm::vec3(0.166667f, 0.598039f, 0.172549f));
			}
			i++;
			//std::cout << "draw" << endl;
		}
		//std::cout << this->hight << endl;
		for (auto pair : this->Circles) {
			//Circle c = pair.second;
			//cout << c.x << " " << c.y << " " << c.r << endl;
			//判断是否发生了撞击事件
			if (!this->meshes[pair.first].flag && if_collision(this->bcircle, pair.second)) {
				if (this->meshes[this->huawen[pair.first]].explode(shader2) && this->meshes[pair.first].explode(shader2)) {
					this->meshes[pair.first].flag = true;
					this->meshes[this->huawen[pair.first]].flag = true;
					cout << "撞到了" << this->cnumber[pair.first] << "号球！" << endl;
				}
				else {
					PlaySound("boom.wav", NULL, SND_ASYNC | SND_FILENAME);
					this->meshes[pair.first].draw(shader2, glm::vec3(1.0f, 1.0f, 1.0f));
					this->meshes[this->huawen[pair.first]].draw(shader2, glm::vec3(0.639216f, 0.478431f, 0.807843f));
				}
			}
		}
	}
	bool loadModel(const std::string& filePath)
	{
		Assimp::Importer importer;
		if (filePath.empty())
		{
			std::cerr << "Error:Model::loadModel, empty model file path." << std::endl;
			return false;
		}
		const aiScene* sceneObjPtr = importer.ReadFile(filePath, 
			aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!sceneObjPtr
			|| sceneObjPtr->mFlags == AI_SCENE_FLAGS_INCOMPLETE
			|| !sceneObjPtr->mRootNode)
		{
			std::cerr << "Error:Model::loadModel, description: " 
				<< importer.GetErrorString() << std::endl;
			return false;
		}
		this->modelFileDir = filePath.substr(0, filePath.find_last_of('/'));
		if (!this->processNode(sceneObjPtr->mRootNode, sceneObjPtr))
		{
			std::cerr << "Error:Model::loadModel, process node failed."<< std::endl;
			return false;
		}
		//计算高度
		this->hight = meshes[21].get_height();
		this->bcircle = this->meshes[21].get_circle(this->hight);
		for (int i = 0; i < 24; i++) {
			if (this->is_pin[i] == 1) {
				this->Circles[i] = this->meshes[i].get_circle(this->hight);
			}
		}
		//手动输入映射关系
		this->cnumber[13] = 1;
		this->cnumber[11] = 2;
		this->cnumber[17] = 3;
		this->cnumber[9] = 4;
		this->cnumber[15] = 5;
		this->cnumber[3] = 6;
		this->cnumber[19] = 7;
		this->cnumber[22] = 8;
		this->cnumber[5] = 9;
		this->cnumber[7] = 10;
		this->huawen[13] = 14;
		this->huawen[11] = 12;
		this->huawen[17] = 18;
		this->huawen[9] = 10;
		this->huawen[15] = 16;
		this->huawen[3] = 4;
		this->huawen[19] = 20;
		this->huawen[22] = 23;
		this->huawen[5] = 6;
		this->huawen[7] = 8;
		return true;
	}
	//移动保龄球
	float move_ball(int code) {
		float returnvalue = 0.0f;
		switch (code)
		{
		case 0:
			if (if_collision(bcircle, -5.906112)) returnvalue = 0.0;
			else {
				Circle temp = this->bcircle;
				temp.x = temp.x - 0.05f;
				if (if_collision(temp, -5.906112)) {
					this->bcircle.x += -5.906112-this->bcircle.x+this->bcircle.r;
					returnvalue = -5.906112 - this->bcircle.x + this->bcircle.r;
				}
				else {
					this->bcircle.x = this->bcircle.x - 0.05f;
					returnvalue = -0.05f;
				}
			}
			break;
		case 1:
			if (if_collision(bcircle, 5.882723)) returnvalue = 0.0;
			else {
				Circle temp = this->bcircle;
				temp.x = temp.x + 0.05f;
				if (if_collision(temp, 5.882723)) {
					this->bcircle.x += 5.882723-this->bcircle.x-this->bcircle.r;
					returnvalue = 5.882723 - this->bcircle.x - this->bcircle.r;
				}
				else {
					this->bcircle.x = this->bcircle.x + 0.05f;
					returnvalue = 0.05f;
				}
			}
			break;
		case 2:
			if (if_collision(bcircle, -9.132236)) returnvalue = 0.0;
			else {
				Circle temp = this->bcircle;
				temp.y = temp.y - 0.05f;
				if (if_collision(temp, -9.132236)) {
					this->bcircle.y += -9.132236-this->bcircle.y+this->bcircle.r;
					returnvalue = -9.132236 - this->bcircle.y + this->bcircle.r;
				}
				else {
					this->bcircle.y = this->bcircle.y - 0.05f;
					returnvalue = -0.05f;
				}
			}
			break;
		case 3:
			if (if_collision(bcircle, 29.099463)) returnvalue = 0.0;
			else {
				Circle temp = this->bcircle;
				temp.y = temp.y + 0.05f;
				if (if_collision(temp, 29.099463)) {
					this->bcircle.y += 29.099463 -this->bcircle.y-this->bcircle.r;
					returnvalue = 29.099463 - this->bcircle.y - this->bcircle.r;
				}
				else {
					this->bcircle.y = this->bcircle.y + 0.05f;
					returnvalue = 0.05f;
				}
			}
			break;
		default:
			break;
			returnvalue = 0.0f;
		}
		return returnvalue;
	}
	~Model()
	{
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			it->final();
		}
	}
private:
	/*
	* 递归处理模型的结点
	*/
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr)
	{
		if (!node || !sceneObjPtr)
		{
			return false;
		}
		// 先处理自身结点
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			// 注意node中的mesh是对sceneObject中mesh的索引
			const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]]; 
			if (meshPtr)
			{
				Mesh meshObj;
				if (this->processMesh(meshPtr, sceneObjPtr, meshObj))
				{
					this->meshes.push_back(meshObj);
				}
			}
		}
		// 处理孩子结点
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			this->processNode(node->mChildren[i], sceneObjPtr);
		}
		return true;
	}
	bool processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr, Mesh& meshObj)
	{
		if (!meshPtr || !sceneObjPtr)
		{
			return false;
		}
		std::vector<Vertex> vertData;
		std::vector<Texture> textures;
		std::vector<GLuint> indices;
		// 从Mesh得到顶点数据、法向量、纹理数据
		for (size_t i = 0; i < meshPtr->mNumVertices; ++i)
		{
			Vertex vertex;
			// 获取顶点位置
			if (meshPtr->HasPositions())
			{
				vertex.position.x = meshPtr->mVertices[i].x;
				vertex.position.y = meshPtr->mVertices[i].y;
				vertex.position.z = meshPtr->mVertices[i].z;
			}
			// 获取纹理数据 目前只处理0号纹理
			if (meshPtr->HasTextureCoords(0))
			{
				vertex.texCoords.x = meshPtr->mTextureCoords[0][i].x;
				vertex.texCoords.y = meshPtr->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			// 获取法向量数据
			if (meshPtr->HasNormals())
			{
				vertex.normal.x = meshPtr->mNormals[i].x;
				vertex.normal.y = meshPtr->mNormals[i].y;
				vertex.normal.z = meshPtr->mNormals[i].z;
			}
			vertData.push_back(vertex);
		}
		// 获取索引数据
		for (size_t i = 0; i < meshPtr->mNumFaces; ++i)
		{
			aiFace face = meshPtr->mFaces[i];
			if (face.mNumIndices != 3)
			{
				std::cerr << "Error:Model::processMesh, mesh not transformed to triangle mesh." << std::endl;
				return false;
			}
			for (size_t j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		// 获取纹理数据
		if (meshPtr->mMaterialIndex >= 0)
		{
			const aiMaterial* materialPtr = sceneObjPtr->mMaterials[meshPtr->mMaterialIndex];
			// 获取diffuse类型
			std::vector<Texture> diffuseTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_DIFFUSE, diffuseTexture);
			textures.insert(textures.end(), diffuseTexture.begin(), diffuseTexture.end());
			// 获取specular类型
			std::vector<Texture> specularTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_SPECULAR, specularTexture);
			textures.insert(textures.end(), specularTexture.begin(), specularTexture.end());
		}
		meshObj.setData(vertData, textures, indices);
		return true;
	}
	/*
	* 获取一个材质中的纹理
	*/
	bool processMaterial(const aiMaterial* matPtr, const aiScene* sceneObjPtr, 
		const aiTextureType textureType, std::vector<Texture>& textures)
	{
		textures.clear();

		if (!matPtr 
			|| !sceneObjPtr )
		{
			return false;
		}
		if (matPtr->GetTextureCount(textureType) <= 0)
		{
			return true;
		}
		for (size_t i = 0; i < matPtr->GetTextureCount(textureType); ++i)
		{
			Texture text;
			aiString textPath;
			aiReturn retStatus = matPtr->GetTexture(textureType, i, &textPath);
			if (retStatus != aiReturn_SUCCESS 
				|| textPath.length == 0)
			{
				std::cerr << "Warning, load texture type=" << textureType
					<< "index= " << i << " failed with return value= "
					<< retStatus << std::endl;
				continue;
			}
			std::string absolutePath = this->modelFileDir + "/" + textPath.C_Str();
			LoadedTextMapType::const_iterator it = this->loadedTextureMap.find(absolutePath);
			if (it == this->loadedTextureMap.end()) // 检查是否已经加载过了
			{
				GLuint textId = TextureHelper::load2DTexture(absolutePath.c_str());
				text.id = textId;
				text.path = absolutePath;
				text.type = textureType;
				textures.push_back(text);
				loadedTextureMap[absolutePath] = text;
			}
			else
			{
				textures.push_back(it->second);
			}
		}
		return true;
	}
private:
	int is_pin[24] = { 0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,1,0};
	map<int, Circle> Circles; //保存每个保龄球对应的半径
	map<int, int> cnumber;//保存每个保龄球对应的编号
	map<int, int> huawen;//保存每个保龄球对应的花纹
	std::vector<Mesh> meshes; // 保存Mesh
	std::string modelFileDir; // 保存模型文件的文件夹路径
	float hight;//保龄球的圆心所对应的高度
	Circle bcircle;//保龄球对应的圆心
	typedef std::map<std::string, Texture> LoadedTextMapType; // key = texture file path
	LoadedTextMapType loadedTextureMap; // 保存已经加载的纹理
};

#endif 