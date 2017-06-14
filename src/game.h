#ifndef GAME_H
#define GAME_H

#include <string>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "sprite_renderer.h"
#include "game_level.h"
#include "ball_object.h"
#include "particle_generator.h"
#include "post_processing.h"
#include "power_up.h"

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

	enum Direction
	{
		UP,
		RIGHT,
		DOWN,
		LEFT
	};
	typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

public:
	// control
	GLboolean _keys[1024];
	GameState _state;	
	GLuint _width, _height;

private:
	// shader
	SpriteRenderer *_spriteRender = nullptr;	

	// levels
	std::vector<GameLevel> _levels;
	GLuint _curlevel;

	// player
	GameObject* _player = nullptr;
	const glm::vec2& _PLAYER_SIZE = { 100, 20 };
	const GLfloat& _PLAY_VELOCITY = { 500.0f };

	// ball
	BallObject* _ball = nullptr;
	const GLfloat _BALL_RADIUS = 12.5;
	const glm::vec2& _INIT_BALL_VELOCITY = { 100.0, -350.0 };

	// particles
	ParticleGenerator* _particleGen = nullptr;

	// postprocessing
	PostProcessing* _effects = nullptr;
	GLfloat _shakeTime = 0.0f;

	// powerups
	std::vector<PowerUp> _powerups;

public:
	void init();
	void processInput(GLfloat dt);
	void update(GLfloat dt);
	void render();

private:
	// collision
	void doCollisions();
	Collision checkCollision(BallObject& a, GameObject& b) const;
	Direction vectorDirection(const glm::vec2& target) const ;

	// reset
	void reset();

	// powerups
	void spawnPowerUps(const GameObject& block);
	void updatePowerUps(GLfloat dt);
	void activePowerup(const PowerUp& powerUp);
};

#endif
