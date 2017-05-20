#ifndef GAME_H
#define GAME_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "sprite_renderer.h"
#include "game_level.h"
#include "ball_object.h"

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
	// control
	GLboolean _keys[1024];
	GameState _state;	
	GLuint _width, _height;

	// shader
	const std::string& _spriteName = "sprite";
	SpriteRenderer *_spriteRender = nullptr;	

	// levels
	std::vector<GameLevel> _levels;
	GLuint _curlevel = 0;

	// player
	GameObject* _player = nullptr;
	const glm::vec2& _playersize = { 100, 20 };
	const GLfloat& _playervelocity = { 500.0f };

	// ball
	BallObject* _ball = nullptr;
	const GLfloat _ballradius = 12.5;
	const glm::vec2& _ballvelocity = { 100.0, -350.0 };

public:
	void init();
	void processInput(GLfloat dt);
	void update(GLfloat dt);
	void render();

private:
	void doCollisions();
	GLboolean checkCollision(BallObject& a, GameObject& b);
};

#endif
