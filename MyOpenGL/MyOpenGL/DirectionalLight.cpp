#include "DirectionalLight.h"

//ƽ�й�
DirectionalLight::DirectionalLight(glm::vec3 _position, glm::vec3 _angles, glm::vec3 _color):
	position(_position),
	angles(_angles),
	color(_color)
{
	UpdateDirection();
}
void DirectionalLight:: UpdateDirection() {
	direction = glm::vec3(0, 0, 0.1f);
	direction = glm::rotateZ(direction, angles.z);
	direction = glm::rotateX(direction, angles.x);
	direction = glm::rotateY(direction, angles.y);
	direction = -1.0f * direction;
}
