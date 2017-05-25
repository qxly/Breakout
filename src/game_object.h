#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"

// Container some single game object entity
class GameObject
{
public:
	GameObject();
	GameObject(const glm::vec2& pos, const glm::vec2& size, const Texture2D& sprite, const glm::vec3& color = glm::vec3(1.0), const glm::vec2& velocity = glm::vec2(0.0));
	virtual ~GameObject();

public:
	glm::vec2 _position, _size, _velocity;
	glm::vec3 _color;
	GLfloat _rotation;
	GLboolean _isSoild;
	GLboolean _destroyed;
	Texture2D _sprite;

public:
	void draw(SpriteRenderer& renderer);
	virtual glm::vec2 center() const;
};

#endif
