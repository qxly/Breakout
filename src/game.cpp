#include "game.h"
#include "resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Game::Game(GLuint width, GLuint height) : _state(GameState::Active), _keys(), _width(width), _height(height)
{

}

Game::~Game()
{
	delete _spriteRender;
	_spriteRender = nullptr;

	delete _player;
	_player = nullptr;

	delete _ball;
	_ball = nullptr;
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
	// left-top is(0,0)
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(_width), static_cast<GLfloat>(_height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader(_spriteName).use().setInteger("image", 0);
	ResourceManager::getShader(_spriteName).setMatrix4("projection", projection);

	// load texture2D
	ResourceManager::loadTexture2D("textures/background.jpg", GL_FALSE, "background");	
	ResourceManager::loadTexture2D("textures/block.png", GL_FALSE, "block");
	ResourceManager::loadTexture2D("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::loadTexture2D("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::loadTexture2D("textures/awesomeface.png", GL_TRUE, "face");

	// load levels
	GameLevel one;
	one.load("levels/one.lvl", this->_width, this->_height * 0.5);
	GameLevel two;
	two.load("levels/two.lvl", this->_width, this->_height * 0.5);
	GameLevel three;
	three.load("levels/three.lvl", this->_width, this->_height * 0.5);
	GameLevel four;
	four.load("levels/four.lvl", this->_width, this->_height * 0.5);
	_levels.push_back(one);
	_levels.push_back(two);
	_levels.push_back(three);
	_levels.push_back(four);
	_curlevel = 0;

	// spriteRender
	_spriteRender = new SpriteRenderer(ResourceManager::getShader(_spriteName));

	// player
	glm::vec2 playerPos = glm::vec2(_width * 0.5 - _playersize.x * 0.5, _height - _playersize.y);
	_player = new GameObject(playerPos, _playersize, ResourceManager::getTexture2D("paddle"));

	// ball
	glm::vec2 ballPos = playerPos + glm::vec2(_playersize.x * 0.5 - _ballradius, -_ballradius * 2.0);
	_ball = new BallObject(ballPos, _ballradius, _ballvelocity, ResourceManager::getTexture2D("face"));
}

void Game::processInput(GLfloat dt)
{	
	if (_state == GameState::Active) {
		GLfloat velocity = _playervelocity * dt;
		if (_keys[GLFW_KEY_A])
		{
			if (_player->_position.x >= 0 )
			{
				_player->_position.x -= velocity;
				if (_ball->_stuck)
				{
					_ball->_position.x -= velocity;
				}
			}
		}
		if(_keys[GLFW_KEY_D])
		{
			if (_player->_position.x <= _width - _player->_size.x)
			{
				_player->_position.x += velocity;
				if (_ball->_stuck)
				{
					_ball->_position.x += velocity;
				}
			}
		}
		if (_keys[GLFW_KEY_SPACE])
		{
			_ball->_stuck = GL_FALSE;
		}


	}
}

void Game::update(GLfloat dt)
{
	_ball->move(dt, _width);
}

void Game::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (_state == GameState::Active)
	{
		_spriteRender->drawSprite(ResourceManager::getTexture2D("background"), glm::vec2(0, 0), glm::vec2(_width, _height), 0.0);

		_levels[_curlevel].draw(*_spriteRender);

		_player->draw(*_spriteRender);

		_ball->draw(*_spriteRender);
	}	
}

