#ifndef GAME_H
#define GAME_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "sprite_renderer.h"
#include "game_level.h"

// Game control
class Game {
public:
	Game(GLuint width, GLuint height);
	~Game();

	enum GameState
	{
		Active,
		Menu,
		Win
	};

public:
	GLboolean _keys[1024];
	GameState _state;	
	GLuint _width, _height;
	const std::string& _spriteName = "sprite";
	SpriteRenderer *_spriteRender = nullptr;
	GameObject* _player = nullptr;
	std::vector<GameLevel> _levels;
	GLuint _curlevel = 0;

	// player
	const glm::vec2& _playersize = { 100, 20 };
	const GLfloat& _playervelocity = { 500.0f };
public:
	void init();
	void processInput(GLfloat dt);
	void update(GLfloat dt);
	void render();
};

#endif
