#pragma once
#include<glm.hpp>
#include<string>
#include<vector>
#include"Shader.h"
#include<gl/glew.h>

//顶点
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};
//纹理
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

//网格类
class Mesh
{
public:
	Mesh(float vertices[]);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> tectures);
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	void Draw(Shader* shader);

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

