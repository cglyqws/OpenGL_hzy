#pragma once
#include<glm.hpp>
#include<gtx/rotate_vector.hpp>

class DirectionalLight
{
public:
	DirectionalLight(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color);
	glm::vec3 position;
	glm::vec3 angles;
	glm::vec3 color;
	glm::vec3 direction=glm::vec3(0,0,0.1f);

	void UpdateDirection();
};

