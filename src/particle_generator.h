#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader;
class Texture2D;
class GameObject;

// a single particle
struct Particle
{
	glm::vec2 _position;
	glm::vec2 _velocity;
	glm::vec4 _color;
	GLfloat _life;
	Particle() :_position(0.0), _velocity(0.0), _color(1.0), _life(0.0) {}
};

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator {
public:
	ParticleGenerator(Shader& shader, Texture2D& texture2D, GLuint amount);

public:
	void update(GLfloat dt, const GameObject& object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0));
	void draw();

private:
	std::vector<Particle> _particles;
	Shader& _shader;
	Texture2D& _texture2D;
	GLuint _amount;

	GLuint _vao;
	GLuint _lastUsedParticle = 0;

private:
	GLuint firstUnusedParticle();
	void respawnParticle(Particle& particle, const GameObject& object, glm::vec2 offset = glm::vec2(0.0));

private:
	void init();
};

#endif // !PARTICLE_GENERATOR_H

