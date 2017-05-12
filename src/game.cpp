#include "game.h"
#include "resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	// load shader
	ResourceManager::loadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, _spriteName);

	// config shader
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(_width), static_cast<GLfloat>(_height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader(_spriteName).use().setInteger("image", 0);
	ResourceManager::getShader(_spriteName).setMatrix4("projection", projection);

	// load texture2D
	ResourceManager::loadTexture2D("textures/awesomeface.png", GL_TRUE, _imageName);

	// spriteRender
	_spriteRender = new SpriteRenderer(ResourceManager::getShader(_spriteName));

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

	_spriteRender->drawSprite(ResourceManager::getTexture2D(_imageName), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0, 1.0, 0.0));
}

