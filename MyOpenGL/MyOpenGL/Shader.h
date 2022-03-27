#pragma once
#include <string>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp> 

using namespace std;

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const char* varyings[], int count);

	string vertexString;
	string fragmentString;
	const char* vertexSource;
	const char* fragmentSource;
	unsigned int ID;

	enum Slot
	{
		DIFFUSE,//   0
		SPECULAR, //enum 中：1
		EMISSION
	};


	void use();
	void SetUniform3f(const char* paramNameString, glm::vec3 param);
	void SetUniform1f(const char* paramNameString, float param);
	void SetUniform1i(const char* paramNameString, int slot);

	// uniform工具函数
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setMat4(const std::string& name, glm::mat4 value)const;
	void setFloat(const std::string& name, float value) const;

private:
	void checkCompileErrors(unsigned int shader, std::string type);

};

