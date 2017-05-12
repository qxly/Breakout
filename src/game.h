#ifndef GAME_H
#define GAME_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "sprite_renderer.h"

class Game {
public:
	Game(GLuint width, GLuint height);
	~Game();

public:
	enum GameState
	{
		Active,
		Menu,
		Win
	};	
	GameState _state;
	GLboolean _keys[1024];
	GLuint _width, _height;

public:
	void init();
	void processInput(GLfloat dt);
	void update(GLfloat dt);
	void render();

private:
	const std::string& _spriteName = "sprite";
	const std::string& _imageName = "face";
	SpriteRenderer *_spriteRender = nullptr;
};

#endif
