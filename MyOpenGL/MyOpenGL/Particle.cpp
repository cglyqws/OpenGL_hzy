#include "Particle.h"


namespace Particle {
	Particle::Particle()
	{
		mCurVBO = 0;
		mCurTransformFeedbackIndex = 1;
		isFirst = true;
		mTimer = 0;
		const GLchar* varyings[5] = { "Type1","Position1",
			"Velocity1","Age1","Size1"
		};//设置TransformFeedback要捕获的输出变量
		mUpdateShader = new Shader("Update.vert", "Update.frag",
			"Update.geom", varyings, 5);
		//设置TransformFeedback缓存能够记录的顶点的数据类型

		mRenderShader = new Shader("Render.vert",
			"Render.frag");
		//设置随机纹理
		InitRandomTexture(512);
		floTexture.loadTexture("flower3.jpg");
		//mSparkTexture.loadTexture("flower3.jpg");
		mRenderShader->use();
		mRenderShader->setInt("snowflower", 0);
		glUseProgram(0);
		InitParticle();
	}


	Particle::~Particle()
	{
	}

	bool Particle::InitParticle()
	{
		Parti particles[MAX_PARTICLES];
		memset(particles, 0, sizeof(particles));
		GenInitLocation(particles, INIT_PARTICLES);
		glGenTransformFeedbacks(2, TransformFeedbacks);
		glGenBuffers(2, ParticleBuffers);
		glGenVertexArrays(2, ParticleArrays);
		for (int i = 0; i < 2; i++)
		{
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TransformFeedbacks[i]);
			glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffers[i]);
			glBindVertexArray(ParticleArrays[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ParticleBuffers[i]);
			//glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);
		glBindVertexArray(0);
		//绑定纹理
		mUpdateShader->use();
		mUpdateShader->setInt("gRandomTexture", 0);
		mUpdateShader->setFloat("MAX_SIZE", MAX_SIZE);
		mUpdateShader->setFloat("MIN_SIZE", MIN_SIZE);
		mUpdateShader->setFloat("MAX_LAUNCH", MAX_LAUNCH);
		mUpdateShader->setFloat("MIN_LAUNCH", MIN_LAUNCH);
		glUseProgram(0);
		return true;
	}

	void Particle::Render(float frametimeMills, glm::mat4& worldMatrix,
		glm::mat4 viewMatrix, glm::mat4& projectMatrix)
	{
		mTimer += frametimeMills * 1000.0f;
		UpdateParticles(frametimeMills * 1000.0f);
		RenderParticles(worldMatrix, viewMatrix, projectMatrix);
		mCurVBO = mCurTransformFeedbackIndex;
		mCurTransformFeedbackIndex = (mCurTransformFeedbackIndex + 1) & 0x1;
	}

	void Particle::UpdateParticles(float frametimeMills)
	{
		mUpdateShader->use();
		mUpdateShader->setFloat("gDeltaTimeMillis", frametimeMills);
		mUpdateShader->setFloat("gTime", mTimer);
		//绑定纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, RandomTexture);

		glEnable(GL_RASTERIZER_DISCARD);//我们渲染到TransformFeedback缓存中去，并不需要光栅化
		glBindVertexArray(ParticleArrays[mCurVBO]);
		glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffers[mCurVBO]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TransformFeedbacks[mCurTransformFeedbackIndex]);

		glEnableVertexAttribArray(0);//type
		glEnableVertexAttribArray(1);//position
		glEnableVertexAttribArray(2);//velocity
		glEnableVertexAttribArray(3);//lifetime
		glEnableVertexAttribArray(4);//size
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, type));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, position));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, velocity));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, lifetimeMills));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, size));
		glBeginTransformFeedback(GL_POINTS);
		if (isFirst)
		{
			glDrawArrays(GL_POINTS, 0, INIT_PARTICLES);
			isFirst = false;
		}
		else {
			glDrawTransformFeedback(GL_POINTS, TransformFeedbacks[mCurVBO]);
		}
		glEndTransformFeedback();
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisable(GL_RASTERIZER_DISCARD);
		//glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Particle::RenderParticles(glm::mat4& worldMatrix,
		glm::mat4& viewMatrix, glm::mat4& projectMatrix)
	{
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		mRenderShader->use();
		mRenderShader->setMat4("model", worldMatrix);
		mRenderShader->setMat4("view", viewMatrix);
		mRenderShader->setMat4("projection", projectMatrix);
		//glBindVertexArray(mParticleArrays[mCurTransformFeedbackIndex]);
		//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,mParticleBuffers[mCurTransformFeedbackIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffers[mCurTransformFeedbackIndex]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, position));
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Parti, size));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floTexture.textureID);
		glDrawTransformFeedback(GL_POINTS, TransformFeedbacks[mCurTransformFeedbackIndex]);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnable(GL_DEPTH_TEST);
	}

	void Particle::InitRandomTexture(unsigned int size)
	{
		srand(time(NULL));
		glm::vec3* pRandomData = new glm::vec3[size];
		for (int i = 0; i < size; i++)
		{
			pRandomData[i].x = float(rand()) / float(RAND_MAX);
			pRandomData[i].y = float(rand()) / float(RAND_MAX);
			pRandomData[i].z = float(rand()) / float(RAND_MAX);
		}
		glGenTextures(1, &RandomTexture);
		glBindTexture(GL_TEXTURE_1D, RandomTexture);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, pRandomData);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		delete[] pRandomData;
		pRandomData = nullptr;
	}

	void Particle::GenInitLocation(Parti particles[], int nums)
	{
		srand(time(NULL));
		for (int x = 0; x < nums; x++) {
			glm::vec3 record(0.0f);
			record.x = (2.0f * float(rand()) / float(RAND_MAX) - 1.0f) * areaLength;
			record.z = (2.0f * float(rand()) / float(RAND_MAX) - 1.0f) * areaLength;
			record.y = fallHeight;
			particles[x].type = PARTICLE_TYPE_LAUNCHER;
			particles[x].position = record;
			particles[x].velocity = (MAX_VELOC - MIN_VELOC)*(float(rand()) / float(RAND_MAX))
				+ MIN_VELOC;//在最大最小速度之间随机选择
			particles[x].size = INIT_SIZE;//发射器粒子大小
			particles[x].lifetimeMills = 0.5f * (float(rand()) / float(RAND_MAX)) + 0.1f;
		}
	}
}
