#pragma once
#include<vector>
#include<string>
#include"Mesh.h"
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<iostream>
#include<map>
#include<assimp/mesh.h>
#include<assimp/material.h>
#include<assimp/texture.h>
#include "stb_image.h"

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
class Model
{
public:
	Model(std::string path);
	std::vector<Mesh> meshes;//
	std::string directory;
	void Draw(Shader* shader);

private:
	void loadModel(std::string path);
	void processNode(aiNode* node,const aiScene * scene );
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int TextureFromFile(const char* path, const string& directory, bool gamma);
};

