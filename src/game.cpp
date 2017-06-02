#include "game.h"
#include "resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using Collision = Game::Collision;

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

	delete _particleGen;
	_particleGen = nullptr;
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
	ResourceManager::loadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, _particleName);

	// config shader
	// left-top is(0,0)
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(_width), static_cast<GLfloat>(_height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader(_spriteName).use().setInteger("image", 0);
	ResourceManager::getShader(_spriteName).setMatrix4("projection", projection);
	ResourceManager::getShader(_particleName).use().setInteger("image", 0);
	ResourceManager::getShader(_particleName).setMatrix4("projection", projection);

	// load texture2D
	ResourceManager::loadTexture2D("textures/background.jpg", GL_FALSE, "background");	
	ResourceManager::loadTexture2D("textures/block.png", GL_FALSE, "block");
	ResourceManager::loadTexture2D("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::loadTexture2D("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::loadTexture2D("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::loadTexture2D("textures/particle.png", GL_TRUE, "particle");

	// load levels
	GLuint levelWidth = static_cast<GLuint>(this->_width);
	GLuint levelHeight = static_cast<GLuint>(this->_height * 0.5);
	GameLevel one;
	one.load("levels/one.lvl", levelWidth, levelHeight);
	GameLevel two;
	two.load("levels/two.lvl", levelWidth, levelHeight);
	GameLevel three;
	three.load("levels/three.lvl", levelWidth, levelHeight);
	GameLevel four;
	four.load("levels/four.lvl", levelWidth, levelHeight);
	_levels.push_back(one);
	_levels.push_back(two);
	_levels.push_back(three);
	_levels.push_back(four);

	_curlevel = 0;

	// spriteRender
	_spriteRender = new SpriteRenderer(ResourceManager::getShader(_spriteName));

	// player/ball
	resetPlayeraAndBall();

	// particle gen
	_particleGen = new ParticleGenerator(ResourceManager::getShader(_particleName), ResourceManager::getTexture2D("particle"), 500);
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

	// collision
	doCollisions();

	// particle
	_particleGen->update(dt, *_ball, 2, glm::vec2(_ball->_radius * 0.5));

	// check loss ball
	if (_ball->_position.y >= _height)
	{
		_levels[_curlevel].reset();
		resetPlayeraAndBall();
	}
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

		if (!_ball->_stuck)
		{
			_particleGen->draw();
		}		

		_ball->draw(*_spriteRender);		
	}	
}

void Game::doCollisions()
{
	if (_ball->_stuck)
	{
		return;
	}

	// collison with bricks
	for (auto& box : _levels[_curlevel]._bricks)
	{
		if (box._destroyed)
		{
			continue;
		}
		auto collision = checkCollision(*_ball, box);
		if (std::get<0>(collision))
		{
			if (!box._isSoild)
			{
				box._destroyed = GL_TRUE;
			}
			auto dir = std::get<1>(collision);
			auto diff_vector = std::get<2>(collision);
			if (dir == Game::LEFT || dir == Game::RIGHT) // Horizontal collison
			{
				_ball->_velocity.x = -_ball->_velocity.x;
				GLfloat penetration = _ball->_radius - std::abs(diff_vector.x);
				if (dir == Game::LEFT)
				{
					_ball->_position.x += penetration;
				}
				else
				{
					_ball->_position.x -= penetration;
				}
			}
			else // vertical collison
			{
				_ball->_velocity.y = -_ball->_velocity.y;
				GLfloat penetration = _ball->_radius - std::abs(diff_vector.y);
				if (dir == UP)
				{
					_ball->_position.y -= penetration;
				}
				else
				{
					_ball->_position.y += penetration;
				}
			}
		}
	}

	// collision with play paddle
	auto coll_player = checkCollision(*_ball, *_player);
	if (std::get<0>(coll_player))
	{
		GLfloat distance = _ball->center().x - _player->center().x;
		GLfloat percentage = static_cast<GLfloat>(distance / (_player->_size.x * 0.5));
		const GLfloat strength = 2.0f;
		const glm::vec2 oldVelocity = _ball->_velocity;
		_ball->_velocity.x = _ballvelocity.x * percentage * strength;
		_ball->_velocity.y = -std::abs(_ball->_velocity.y);
		_ball->_velocity = glm::normalize(_ball->_velocity) * glm::length(oldVelocity);
	}
}

Collision Game::checkCollision(BallObject& a, GameObject& b) const
{
	const glm::vec2& centerA = a.center();
	glm::vec2 aabb_half_extents(b._size.x * 0.5, b._size.y * 0.5);
	const glm::vec2& centerB = b.center();

	glm::vec2 difference = centerA - centerB;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	glm::vec2 closed = centerB + clamped;
	difference = closed - centerA;
	
	if (glm::length(difference) <= a._radius)
	{
		return std::make_tuple(GL_TRUE, vectorDirection(difference), difference);
	}
	else
	{
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0.0, 0.0));
	}
}

Game::Direction Game::vectorDirection(const glm::vec2& target) const
{
	const glm::vec2 compass[] = {
		glm::vec2(0.0, 1.0), // up
		glm::vec2(1.0, 0.0), // right
		glm::vec2(0.0, -1.0), // down
		glm::vec2(-1.0, 0.0) // left
	};

	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}

	return (Direction)best_match;
}

void Game::resetPlayeraAndBall()
{
	// player
	glm::vec2 playerPos = glm::vec2(_width * 0.5 - _playersize.x * 0.5, _height - _playersize.y);
	if (nullptr == _player)
	{
		_player = new GameObject(playerPos, _playersize, ResourceManager::getTexture2D("paddle"));
	}
	else
	{
		_player->_position = playerPos;
	}

	// ball
	glm::vec2 ballPos = playerPos + glm::vec2(_playersize.x * 0.5 - _ballradius, -_ballradius * 2.0);
	if (nullptr == _ball)
	{
		_ball = new BallObject(ballPos, _ballradius, _ballvelocity, ResourceManager::getTexture2D("face"));
	}
	else
	{
		_ball->_position = ballPos;
		_ball->_velocity = _ballvelocity;
		_ball->_stuck = GL_TRUE;
	}
}

