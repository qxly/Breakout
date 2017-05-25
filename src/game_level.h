#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

// Hold all tiles of level, and load/render levels
class GameLevel
{
public:
	GameLevel();
	~GameLevel();

public:
	std::vector<GameObject> _bricks;

private:
	std::string _file;
	GLuint _levelWidth;
	GLuint _levelHeight;

public:
	void load(const GLchar* file, GLuint levelWidth, GLuint levelHeight);
	void reset();

	void draw(SpriteRenderer& renderer);

	GLboolean isCompleted();

private:
	void init(const std::vector<std::vector<GLuint>>& tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif

