#version 330 core
layout (location = 0) in vec3 aPos; // λ�ñ���������λ��ֵΪ0
////layout (location = 1) in vec3 aColors; // λ�ñ���������λ��ֵΪ0
//layout (location = 1) in vec3 aNormal; 
//layout(location=2)in vec2 aTexCoord;
//layout(location=3)in float size;
layout (location = 1)in float size;

out vec3 FragPos; //ƬԪλ��

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

////out vec2 TexCoord;
////out vec4 vertexColor; // ΪƬ����ɫ��ָ��һ����ɫ���
//out vec3 Normal;  //������
////out vec2 TexCoord;

void main()
{
    FragPos=aPos;
	gl_PointSize=size;
	gl_Position = projection * view * model * vec4(aPos, 1.0); 
   ////FragPos=vec3(model * vec4(aPos, 1.0f));//ÿ��ƬԪ��λ��
   ////Normal=mat3(model) * aNormal ;   //�����в��ȱ�����ʱ���ᵼ�·�������б
   //Normal=mat3(transpose(inverse(model))) *aNormal;
   ////TexCoord = aTexCoord;

}
