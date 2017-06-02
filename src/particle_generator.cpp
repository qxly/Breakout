#include "particle_generator.h"
#include "shader.h"
#include "texture.h"
#include "game_object.h"

ParticleGenerator::ParticleGenerator(Shader& shader, Texture2D& texture2D, GLuint amount) : _shader(shader), _texture2D(texture2D), _amount(amount)
{
	init();
}

void ParticleGenerator::update(GLfloat dt, const GameObject& object, GLuint newParticles, glm::vec2 offset /*= glm::vec2(0.0)*/)
{
	// add new particles
	for (GLuint i = 0; i < newParticles; i++)
	{
		GLuint index = firstUnusedParticle();
		respawnParticle(_particles[index], object, offset);
	}

	// update particles
	for (auto& it : _particles)
	{
		it._life -= dt;
		if (it._life > 0.0)
		{
			it._position -= it._velocity * dt;
			it._color -= dt * 2.5;
		}
	}
}

void ParticleGenerator::draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	_shader.use();
	for (const auto& it : _particles)
	{
		if (it._life > 0.0)
		{
			_shader.setVector2f("offset", it._position);
			_shader.setVector4f("color", it._color);
			_texture2D.bind();
			glBindVertexArray(_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint ParticleGenerator::firstUnusedParticle()
{
	for (GLuint i = _lastUsedParticle; i < _amount; i++)
	{
		if (_particles[i]._life <= 0.0)
		{
			_lastUsedParticle = i;
			return i;
		}
	}

	for (GLuint i = 0; i < _lastUsedParticle; i++)
	{
		if (_particles[i]._life <= 0.0)
		{
			_lastUsedParticle = i;
			return i;
		}
	}

	_lastUsedParticle = 0;

	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, const GameObject& object, glm::vec2 offset /*= glm::vec2(0.0)*/)
{
	GLfloat random = static_cast<GLfloat>((rand() % 100 - 50.0) / 10.0);
	particle._position = object._position + offset + random;
	GLfloat c = static_cast<GLfloat>(0.5 + rand() % 100 / 100.0);
	particle._color = glm::vec4(c, c, c, 1.0);
	particle._life = 1.0;
	particle._velocity = object._velocity * 0.1f;
}

void ParticleGenerator::init()
{
	GLuint vbo;
	GLfloat quad[] = {
		0.0, 1.0, 0.0, 1.0, 
		1.0, 0.0, 1.0, 0.0, 
		0.0, 0.0, 0.0, 0.0, 

		0.0, 1.0, 0.0, 1.0, 
		1.0, 1.0, 1.0, 1.0, 
		1.0, 0.0, 1.0, 0.0
	};
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//init particles
	_particles.resize(_amount);
}
