#ifndef POWERUP_H
#define POWERUP_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "game_object.h"

class PowerUp : public GameObject {
public:
	enum Type;
	PowerUp(Type type, const glm::vec2& positon, const Texture2D& sprite, const glm::vec3& color, GLfloat duration) : GameObject(positon, { 60.0, 20.0 }, sprite, color, { 0.0, 150.0 }), _type(type), _duration(duration), _activated(GL_FALSE) {}

public:
	enum Type{
		Speed,
		Sticky,
		PassThrough,
		PadSizeIncrease,
		Confuse,
		Chaos
	};

	Type _type;
	GLfloat _duration;
	GLboolean _activated;
};
#endif
