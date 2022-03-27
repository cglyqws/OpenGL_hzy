#pragma once
#include"Shader.h"
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp> 


class Material
{
public:
	Shader* shader;
	unsigned int diffuse;
	glm::vec3 ambient;
	unsigned int specular;
	unsigned int emission;
	float shininess;

	Material(Shader* _shader, unsigned int _diffuse, unsigned int _specular, unsigned int emission, glm::vec3 _ambient, float _shininess);

};

