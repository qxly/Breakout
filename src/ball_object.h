#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "game_object.h"

class BallObject : public GameObject {
public:
	BallObject();
	BallObject(const glm::vec2& pos, GLfloat radius, const glm::vec2& velocity, const Texture2D& sprite);

public:
	GLfloat _radius;
	GLboolean _stuck;

public:
	glm::vec2 move(GLfloat dt, GLuint window_width);
	void reset(const glm::vec2& position, const glm::vec2& velocity);
	virtual glm::vec2 center() const override;
};
#endif
