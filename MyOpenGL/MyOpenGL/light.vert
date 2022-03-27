#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0
////layout (location = 1) in vec3 aColors; // 位置变量的属性位置值为0
//layout (location = 1) in vec3 aNormal; 
//layout(location=2)in vec2 aTexCoord;
//layout(location=3)in float size;
layout (location = 1)in float size;

out vec3 FragPos; //片元位置

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

////out vec2 TexCoord;
////out vec4 vertexColor; // 为片段着色器指定一个颜色输出
//out vec3 Normal;  //法向量
////out vec2 TexCoord;

void main()
{
    FragPos=aPos;
	gl_PointSize=size;
	gl_Position = projection * view * model * vec4(aPos, 1.0); 
   ////FragPos=vec3(model * vec4(aPos, 1.0f));//每个片元的位置
   ////Normal=mat3(model) * aNormal ;   //当进行不等比缩放时，会导致法向量歪斜
   //Normal=mat3(transpose(inverse(model))) *aNormal;
   ////TexCoord = aTexCoord;

}
