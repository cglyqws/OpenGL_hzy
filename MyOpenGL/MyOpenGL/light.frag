#version 330 core
//in vec4 vertexColor; 
//in vec2 TexCoord;
in vec3 FragPos; //ƬԪλ��
out vec4 FragColor;
uniform sampler2D flowers;

in vec3 Normal;  //������
in vec2 TexCoord;
//uniform sampler2D ourTexture;  //�������һ���زĽ��������������ز�
//uniform sampler2D ourFace;  //�������һ���زĽ��������������ز�
//uniform float mixValue;
uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightDirUniform;//���������Ĺ��շ���ƽ�й⣩

struct Material{    //���ʽṹ��
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct LightDirectional{   //ƽ�й� ����⣡
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//��ƬԪ���� ָ���ķ�����ƽ�й�ĸ�������ֵӦ���ɹ�Դ�����ṩ��ÿ��ƬԪ������Ӧ������ͬ��ָ����������Ӧ����ƬԪ��λ�þ���
};//��ƬԪλ�þ������ǵ��Դ�ĸ���

struct LightPonit{  //���Դ 
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//û�ã�������
	float constant;
	float linear;
	float quadratic;
};

struct LightSpot{   //�۹��
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//û�ã�������
	float constant;
	float linear;
	float quadratic;

	float cosPhyInner;
	float cosPhyOutter;
};

//д�ýṹ��֮��Ȼ���������һ��uniform��
uniform Material material;
uniform LightDirectional lightD;
uniform LightPonit lightP0; 
uniform LightPonit lightP1; 
uniform LightPonit lightP2; 
uniform LightPonit lightP3; 
uniform LightSpot lightS;

vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera){  //���弸����Դ��������ɫ�ĺ���
																	//��������һ����ɫ��vec3��
															    	//����ɫ ���������������
	//diffuse  max(dot(L,N),0)   
	float diffuseAmount = max( dot(light.dirToLight,uNormal) ,0 );//light.dirToLight ����ƬԪ�����Ķ�����ͬ���ķ���
	vec3 diffColor = diffuseAmount * light.color * texture(material.diffuse,TexCoord).rgb;

	//specular  
	vec3 R=normalize(reflect(-light.dirToLight,uNormal));
	float specIntensity= pow( max( dot( R , dirToCamera) ,0 ), material.shininess);
	vec3 specColor=specIntensity*light.color * texture(material.specular,TexCoord).rgb;

	vec3 result=diffColor+specColor;
	return result;//����ֵ
}

//���Դ�� ��ɫ���㺯��
vec3 CalcLightDirectional(LightPonit light,vec3 uNormal,vec3 dirToCamera){

	//attenuation ˥����
	float distanc=length(light.pos-FragPos);//length ��ģֵ
	float attenuation =1/(light.constant+light.linear*distanc+light.quadratic*(distanc*distanc));//����ԽԶ�����ԴԽ��

	//diffuse 
	float diffuseAmount = max( dot( normalize(light.pos - FragPos),uNormal) ,0 ) * attenuation;
										//����ƬԪָ����Դ ����(light.pos - FragPos)
	vec3 diffColor = diffuseAmount * light.color * texture(material.diffuse,TexCoord).rgb;

	//specular
	vec3 R=normalize(reflect(-normalize(light.pos - FragPos),uNormal));//�Ӹ��� ��ΪҪreflect�� �Ǵӹ�Դ����ָ��ƬԪ
	float specIntensity= pow( max( dot( R , dirToCamera) ,0 ), material.shininess) * attenuation;
	vec3 specColor=specIntensity*light.color * texture(material.specular,TexCoord).rgb;


	vec3 result=diffColor+specColor;
	return result;
}

//�۹�Դ�� ��ɫ���㺯��
vec3 CalcLightDirectional(LightSpot light,vec3 uNormal,vec3 dirToCamera){
	float distanc=length(light.pos-FragPos);//length ��ģֵ
	float attenuation =1/(light.constant+light.linear*distanc+light.quadratic*(distanc*distanc));//����ԽԶ�����ԴԽ��

	float cosTheta=dot( normalize(FragPos-light.pos),-light.dirToLight);
	float spotRatio;
	if(cosTheta>lightS.cosPhyInner){
		//inside
		spotRatio=1.0f;
		}
	else if(cosTheta>lightS.cosPhyOutter){
		//middle
		spotRatio=1.0f-(cosTheta-lightS.cosPhyInner)/(lightS.cosPhyOutter-lightS.cosPhyInner);
	}
	else{
		//outside
		spotRatio=0;
	}
		//diffuse 
	float diffuseAmount = max( dot( normalize(light.pos - FragPos),uNormal) ,0 )*attenuation*spotRatio;
										//����ƬԪָ����Դ ����(light.pos - FragPos)
	vec3 diffColor = diffuseAmount * light.color * texture(material.diffuse,TexCoord).rgb;

	//specular
	vec3 R=normalize(reflect(-normalize(light.pos - FragPos),uNormal));//�Ӹ��� ��ΪҪreflect�� �Ǵӹ�Դ����ָ��ƬԪ
	float specIntensity= pow( max( dot( R , dirToCamera) ,0 ), material.shininess)*attenuation*spotRatio;
	vec3 specColor=specIntensity*light.color * texture(material.specular,TexCoord).rgb;

	vec3 result=diffColor+specColor;
	return result;
}


void main()
{
	vec3 finalResult=vec3(0,0,0);  //��ʼ��
	vec3 uNormal=normalize(Normal);
	vec3 dirToCamera=normalize(cameraPos-FragPos);//ָ��camera���������ǵ�һ��Ҫ��׼�� ��ֻҪ���򣬲�Ҫ���ȣ�

	//ƽ�й�
	finalResult+=CalcLightDirectional(lightD,uNormal,dirToCamera); //��ɫ���ӣ��ѹ�Դ����ɫ����ȥ
	//����������� ��Դ�� ƬԪ������ �� ָ��camera��һ������
	
	//���Դ
	finalResult+=CalcLightDirectional(lightP0,uNormal,dirToCamera);
	finalResult+=CalcLightDirectional(lightP1,uNormal,dirToCamera);
	finalResult+=CalcLightDirectional(lightP2,uNormal,dirToCamera);
	finalResult+=CalcLightDirectional(lightP3,uNormal,dirToCamera);

	finalResult+=CalcLightDirectional(lightS,uNormal,dirToCamera);

	vec4 texColor=texture(flowers,gl_PointCoord);//gl_PointCoord ����Ⱦģʽ��Ӧ����������
	texColor.a=texColor.a;

	FragColor=vec4(finalResult,1.0);
	FragColor=texColor;

	//FragColor=vec4(1.0,1.0,1.0,1.0);
}
