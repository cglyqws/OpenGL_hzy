#pragma once
#include<glm.hpp>
#include<gtx/rotate_vector.hpp>

class LightSpot
{
public:
	LightSpot(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f));

	void UpdateDirection();
	glm::vec3 position;
	glm::vec3 angles;
	glm::vec3 direction = glm::vec3(0, 0, 1.0f);
	glm::vec3 color;

	float cosPhyInner = 0.9f; //把角设小一点  最大张角设小一点//角越小，越接近1
	float cosPhyOutter = 0.8f;

	float constant;
	float linear;		 //一次项衰减值
	float quadratic;	//二次项衰减值

	
};