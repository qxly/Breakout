#include "game.h"
#include "resource_manager.h"

Game::Game(GLuint width, GLuint height) : _state(GameState::Active), _keys(), _width(width), _height(height)
{

}

Game::~Game()
{

}

void Game::init()
{
	// OpenGL configuration
	glViewport(0, 0, _width, _height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::processInput(GLfloat dt)
{

}

void Game::update(GLfloat dt)
{

}

void Game::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

