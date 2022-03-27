#include "Model.h"

Model::Model(std::string path)
{
	loadModel(path);//自定义函数
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene||scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode)
	{
		std::cout << "Assimp error" << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('\\')); //substr()函数 把本身字符串取出 始末位置间隔的挖出来
	//std::cout << "Success!"<<directory << std::endl;
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//std::cout << node->mName.data <<std:: endl;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}
//将Assimp的数据解析到创建的Mesh类中
//将aiMesh对象转化为我们自己的网格对象
//访问网格的相关属性并将它们存储到自己的对象中
Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex>vertices;
	vector<unsigned int>indices;
	vector<Texture>textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//处理顶点位置、法线和纹理坐标
		//顶点位置 ( Assimp的顶点位置数组为mVertices )
		//glm::vec3 vector;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		//法线坐标
		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;
		//纹理坐标 UV
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);
		}
		//现在vertex结构体已经填好了需要的顶点属性。在迭代的最后将它压入vertices这个vector尾部。
		//这个过程会对每个网格的顶点都重复一遍。
		vertices.push_back(vertex);
	}
		//索引
		//每个网格都有一个面face数组 一个面包含多个索引遍历所有面并储存面对索引到indices则会个vector中
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)//有几个面
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		//材质 获取网格真正的材质，需索引场景的mMaterials数组
		//网格材质索引位于它的mMaterialIndex属性中
		if (mesh->mMaterialIndex>=0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}
	return Mesh(vertices,indices,textures);
}

void Model::Draw(Shader* shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	//该函数返回Texture结构体的vector
	//遍历给定纹理类型的所有纹理位置 获取纹理的文件位置 并加载和生成纹理，将信息存储在一个Vertex结构体中
	vector<Texture>textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		//texture.id = TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.path = str.C_Str();//转化成string
		textures.push_back(texture);
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const string& directory, bool gamma)
{

	string filename = string(path);
	filename = directory + '\\' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;

}
