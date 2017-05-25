#include "game_object.h"


GameObject::GameObject():_position(0.0, 0.0), _size(1.0, 1.0), _velocity(0.0, 0.0), _color(1.0), _rotation(0.0), _sprite(), _isSoild(GL_FALSE), _destroyed(GL_FALSE)
{
}

GameObject::GameObject(const glm::vec2& pos, const glm::vec2& size, const Texture2D& sprite, const glm::vec3& color /*= glm::vec3(1.0)*/, const glm::vec2& velocity /*= glm::vec2(0.0)*/) : _position(pos), _size(size), _sprite(sprite), _color(color), _velocity(velocity), _rotation(0.0), _isSoild(GL_FALSE), _destroyed(GL_FALSE)
{
}

GameObject::~GameObject()
{
}

void GameObject::draw(SpriteRenderer& renderer)
{
	renderer.drawSprite(_sprite, _position, _size, _rotation, _color);
}

glm::vec2 GameObject::center() const
{
	return _position + _size * glm::vec2(0.5);
}

