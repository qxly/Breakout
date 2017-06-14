#include "ball_object.h"

BallObject::BallObject(const glm::vec2& pos, GLfloat radius, const glm::vec2& velocity, const Texture2D& sprite) : GameObject(pos, glm::vec2(radius * 2.0, radius * 2.0), sprite, glm::vec3(1.0), velocity), _radius(radius), _stuck(GL_TRUE), _sticky(GL_FALSE), _passThrough(GL_FALSE)
{

}

glm::vec2 BallObject::move(GLfloat dt, GLuint window_width)
{
	if (!_stuck)
	{
		_position += _velocity * dt;
		if (_position.x <= 0.0)
		{
			_velocity.x = -_velocity.x;
			_position.x = 0.0;
		}
		else if (_position.x + _size.x >= window_width)
		{
			_velocity.x = -_velocity.x;
			_position.x = window_width - _size.x;
		}
		else if (_position.y <= 0.0)
		{
			_velocity.y = -_velocity.y;
			_position.y = 0.0;
		}
	}

	return _position;
}

void BallObject::reset(const glm::vec2& position, const glm::vec2& velocity)
{
	_position = position;
	_velocity = velocity;
	_stuck = GL_TRUE;
	_sticky = GL_FALSE;
	_passThrough = GL_FALSE;
}

glm::vec2 BallObject::center() const
{
	return _position + glm::vec2(_radius);
}

