#version 330 core
//in vec4 vertexColor; 
//in vec2 TexCoord;
in vec3 FragPos; //片元位置
out vec4 FragColor;
uniform sampler2D flowers;

in vec3 Normal;  //法向量
in vec2 TexCoord;
//uniform sampler2D ourTexture;  //从外面灌一张素材进来，调用这张素材
//uniform sampler2D ourFace;  //从外面灌一张素材进来，调用这张素材
//uniform float mixValue;
uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightDirUniform;//外面灌进来的关照方向（平行光）

struct Material{    //材质结构体
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct LightDirectional{   //平行光 定向光！
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//从片元出发 指向光的方向；在平行光的概念里，这个值应该由光源本身提供，每个片元看到的应该是相同的指向向量，不应该由片元的位置决定
};//由片元位置决定的是点光源的概念

struct LightPonit{  //点光源 
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//没用，先留着
	float constant;
	float linear;
	float quadratic;
};

struct LightSpot{   //聚光灯
	vec3 pos;
	vec3 color;
	vec3 dirToLight;//没用，先留着
	float constant;
	float linear;
	float quadratic;

	float cosPhyInner;
	float cosPhyOutter;
};

//写好结构体之后，然后具体整成一个uniform；
uniform Material material;
uniform LightDirectional lightD;
uniform LightPonit lightP0; 
uniform LightPonit lightP1; 
uniform LightPonit lightP2; 
uniform LightPonit lightP3; 
uniform LightSpot lightS;

vec3 CalcLightDirectional(LightDirectional light,vec3 uNormal,vec3 dirToCamera){  //定义几个光源各自算颜色的函数
																	//函数返还一个颜色（vec3）
															    	//算颜色 都用这个函数来算
	//diffuse  max(dot(L,N),0)   
	float diffuseAmount = max( dot(light.dirToLight,uNormal) ,0 );//light.dirToLight 所有片元看到的都会是同样的方向
	vec3 diffColor = diffuseAmount * light.color * texture(material.diffuse,TexCoord).rgb;

	//specular  
	vec3 R=normalize(reflect(-light.dirToLight,uNormal));
	float specIntensity= pow( max( dot( R , dirToCamera) ,0 ), material.shininess);
	vec3 specColor=specIntensity*light.color * texture(material.specular,TexCoord).rgb;

	vec3 result=diffColor+specColor;
	return result;//返还值
}

//点光源的 颜色计算函数
vec3 CalcLightDirectional(LightPonit light,vec3 uNormal,vec3 dirToCamera){

	//attenuation 衰减量
	float distanc=length(light.pos-FragPos);//length 求模值
	float attenuation =1/(light.constant+light.linear*distanc+light.quadratic*(distanc*distanc));//距离越远，点光源越暗

	//diffuse 
	float diffuseAmount = max( dot( normalize(light.pos - FragPos),uNormal) ,0 ) * attenuation;
										//！！片元指向点光源 所以(light.pos - FragPos)
	vec3 diffColor = diffuseAmount * light.color * texture(material.diffuse,TexCoord).rgb;

	//specular
	vec3 R=normalize(reflect(-normalize(light.pos - FragPos),uNormal));//加负号 因为要reflect话 是从光源出发指向片元
	float specIntensity= pow( max( dot( R , dirToCamera) ,0 ), material.shininess) * attenuation;
	vec3 specColor=specIntensity*light.color * texture(material.specular,TexCoord).rgb;


	vec3 result=diffColor+specColor;
	return result;
}

//聚光源的 颜色计算函数
vec3 CalcLightDirectional(LightSpot light,vec3 uNormal,vec3 dirToCamera){
	float distanc=length(light.pos-FragPos);//length 求模值
	float attenuation =1/(light.constant+light.linear*distanc+light.quadratic*(distanc*distanc));//距离越远，点光源越暗

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
										//！！片元指向点光源 所以(light.pos - FragPos)
	vec3 diffColor = diffuseAmount * light.color * texture(material.diffuse,TexCoord).rgb;

	//specular
	vec3 R=normalize(reflect(-normalize(light.pos - FragPos),uNormal));//加负号 因为要reflect话 是从光源出发指向片元
	float specIntensity= pow( max( dot( R , dirToCamera) ,0 ), material.shininess)*attenuation*spotRatio;
	vec3 specColor=specIntensity*light.color * texture(material.specular,TexCoord).rgb;

	vec3 result=diffColor+specColor;
	return result;
}


void main()
{
	vec3 finalResult=vec3(0,0,0);  //初始化
	vec3 uNormal=normalize(Normal);
	vec3 dirToCamera=normalize(cameraPos-FragPos);//指向camera的向量。记得一定要标准化 （只要方向，不要长度）

	//平行光
	finalResult+=CalcLightDirectional(lightD,uNormal,dirToCamera); //颜色叠加，把光源的颜色加上去
	//参数：这个灯 光源； 片元法向量 ； 指向camera的一个向量
	
	//点光源
	finalResult+=CalcLightDirectional(lightP0,uNormal,dirToCamera);
	finalResult+=CalcLightDirectional(lightP1,uNormal,dirToCamera);
	finalResult+=CalcLightDirectional(lightP2,uNormal,dirToCamera);
	finalResult+=CalcLightDirectional(lightP3,uNormal,dirToCamera);

	finalResult+=CalcLightDirectional(lightS,uNormal,dirToCamera);

	vec4 texColor=texture(flowers,gl_PointCoord);//gl_PointCoord 点渲染模式对应点像素坐标
	texColor.a=texColor.a;

	FragColor=vec4(finalResult,1.0);
	FragColor=texColor;

	//FragColor=vec4(1.0,1.0,1.0,1.0);
}
