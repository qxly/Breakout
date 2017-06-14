#include "game.h"

#include <algorithm>

#include "resource_manager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <irrklang/irrKlang.h>

using Collision = Game::Collision;
using namespace irrklang;

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

	delete _effects;
	_effects = nullptr;

	_soundEngine->drop();
}

void Game::init()
{
	// OpenGL configuration
	glViewport(0, 0, _width, _height);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// load shader
	ResourceManager::loadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
	ResourceManager::loadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");
	ResourceManager::loadShader("shaders/postprocessing.vs", "shaders/postprocessing.frag", nullptr, "postprocessing");

	// config shader
	// left-top is(0,0)
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(_width), static_cast<GLfloat>(_height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use().setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);
	ResourceManager::getShader("particle").use().setInteger("image", 0);
	ResourceManager::getShader("particle").setMatrix4("projection", projection);

	// load texture2D
	ResourceManager::loadTexture2D("textures/background.jpg", GL_FALSE, "background");
	ResourceManager::loadTexture2D("textures/block.png", GL_FALSE, "block");
	ResourceManager::loadTexture2D("textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::loadTexture2D("textures/paddle.png", GL_TRUE, "paddle");
	ResourceManager::loadTexture2D("textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::loadTexture2D("textures/particle.png", GL_TRUE, "particle");
	ResourceManager::loadTexture2D("textures/powerup_speed.png", GL_TRUE, "powerup_speed");
	ResourceManager::loadTexture2D("textures/powerup_sticky.png", GL_TRUE, "powerup_sticky");
	ResourceManager::loadTexture2D("textures/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
	ResourceManager::loadTexture2D("textures/powerup_increase.png", GL_TRUE, "powerup_increase");
	ResourceManager::loadTexture2D("textures/powerup_confuse.png", GL_TRUE, "powerup_confuse");
	ResourceManager::loadTexture2D("textures/powerup_chaos.png", GL_TRUE, "powerup_chaos");

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
	_levels.emplace_back(one);
	_levels.emplace_back(two);
	_levels.emplace_back(three);
	_levels.emplace_back(four);

	_curlevel = 0;

	// spriteRender
	_spriteRender = new SpriteRenderer(ResourceManager::getShader("sprite"));

	// particle gen
	_particleGen = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getTexture2D("particle"), 500);

	// player/ball//postprocessing
	reset();

	//audio
	_soundEngine = createIrrKlangDevice();
	_soundEngine->play2D("audio/breakout.mp3", GL_TRUE);
}

void Game::processInput(GLfloat dt)
{
	if (_state == GameState::Active) {
		GLfloat velocity = _PLAY_VELOCITY * dt;
		if (_keys[GLFW_KEY_A])
		{
			if (_player->_position.x >= 0)
			{
				_player->_position.x -= velocity;
				if (_ball->_stuck)
				{
					_ball->_position.x -= velocity;
				}
			}
		}
		if (_keys[GLFW_KEY_D])
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
	_particleGen->update(dt, *_ball, 2, glm::vec2(_ball->_radius * 0.5f));

	// powerup
	updatePowerUps(dt);

	// shake
	if (_shakeTime > 0.0f)
	{
		_shakeTime -= dt;
		if (_shakeTime < 0.0f)
		{
			_effects->_shake = GL_FALSE;
		}
	}

	// check loss ball
	if (_ball->_position.y >= _height)
	{
		_levels[_curlevel].reset();
		reset();
	}
}

void Game::render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (_state == GameState::Active)
	{
		_effects->beginRender();
		_spriteRender->drawSprite(ResourceManager::getTexture2D("background"), glm::vec2(0, 0), glm::vec2(_width, _height), 0.0);
		_levels[_curlevel].draw(*_spriteRender);
		_player->draw(*_spriteRender);
		for (auto& it : _powerups)
		{
			if (!it._destroyed)
			{
				it.draw(*_spriteRender);
			}
		}
		_particleGen->draw();
		_ball->draw(*_spriteRender);
		_effects->endRender();

		_effects->render(static_cast<GLfloat>(glfwGetTime()));
	}
}

void Game::doCollisions()
{
	// ball with bricks
	if (!_ball->_stuck)
	{
		for (auto& box : _levels[_curlevel]._bricks)
		{
			if (box._destroyed)
			{
				continue;
			}
			Collision collision = checkCollision(*_ball, box);
			if (!std::get<0>(collision))
			{
				continue;
			}
			if (box._isSoild) // if solid, enable shake
			{
				_shakeTime = 0.05f;
				_effects->_shake = GL_TRUE;
				_soundEngine->play2D("audio/solid.wav", GL_FALSE);
			}
			else
			{
				box._destroyed = GL_TRUE;
				spawnPowerUps(box);
				_soundEngine->play2D("audio/bleep.mp3", GL_FALSE);
			}
			if (!(_ball->_passThrough && !box._isSoild))
			{
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
	}

	// powerup with paddle
	auto checkCollisionAABB = [](GameObject& a, GameObject& b)	// AABB collision
	{
		GLboolean collisionX = a._position.x + a._size.x >= b._position.x && b._position.x + b._size.x >= a._position.x;
		GLboolean collisionY = a._position.y + a._size.y >= b._position.y && b._position.y + b._size.y >= a._position.y;

		return collisionX && collisionY;
	};
	for (auto& it : _powerups)
	{
		if (it._destroyed)
		{
			continue;
		}
		if (it._position.y >= _height)
		{
			it._destroyed = GL_TRUE;
		}
		if (checkCollisionAABB(*_player, it))
		{
			it._destroyed = GL_TRUE;
			it._activated = GL_TRUE;
			activePowerup(it);
			_soundEngine->play2D("audio/powerup.mp3", GL_FALSE);
		}
	}

	// ball with paddle
	if (!_ball->_stuck)
	{
		auto coll_player = checkCollision(*_ball, *_player);
		if (std::get<0>(coll_player))
		{
			GLfloat distance = _ball->center().x - _player->center().x;
			GLfloat percentage = static_cast<GLfloat>(distance / (_player->_size.x * 0.5));
			const GLfloat strength = 2.0f;
			const glm::vec2 oldVelocity = _ball->_velocity;
			_ball->_velocity.x = _INIT_BALL_VELOCITY.x * percentage * strength;
			_ball->_velocity.y = -std::abs(_ball->_velocity.y);
			_ball->_velocity = glm::normalize(_ball->_velocity) * glm::length(oldVelocity);

			_ball->_stuck = _ball->_sticky;
			_soundEngine->play2D("audio/bleep.wav", GL_FALSE);
		}
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

void Game::reset()
{
	// player
	glm::vec2 playerPos = glm::vec2(_width * 0.5 - _PLAYER_SIZE.x * 0.5, _height - _PLAYER_SIZE.y);
	if (nullptr == _player)
	{
		_player = new GameObject(playerPos, _PLAYER_SIZE, ResourceManager::getTexture2D("paddle"));
	}
	else
	{
		_player->_position = playerPos;
		_player->_size = _PLAYER_SIZE;
		_player->_color = glm::vec3(1.0);
	}

	// ball
	glm::vec2 ballPos = playerPos + glm::vec2(_PLAYER_SIZE.x * 0.5 - _BALL_RADIUS, -_BALL_RADIUS * 2.0);
	if (nullptr == _ball)
	{
		_ball = new BallObject(ballPos, _BALL_RADIUS, _INIT_BALL_VELOCITY, ResourceManager::getTexture2D("face"));
	}
	else
	{
		_ball->reset(ballPos, _INIT_BALL_VELOCITY);
		_ball->_color = glm::vec3(1.0);
	}

	// postprpcessing
	if (nullptr == _effects)
	{
		_effects = new PostProcessing(ResourceManager::getShader("postprocessing"), _width, _height);
	}
	else
	{
		_effects->_chaos = GL_FALSE;
		_effects->_confuse = GL_FALSE;
	}

	// powerups
	if (!_powerups.empty())
	{
		_powerups.clear();
	}
}

void Game::spawnPowerUps(const GameObject& block) {
	auto shouldSpawn = [](GLuint chance) {
		if (0 == chance)
		{
			return false;
		}
		else
		{
			return (rand() % chance) == 0;
		}		
	};

	GLuint change = 75;
	if (shouldSpawn(change))
	{
		_powerups.emplace_back(PowerUp(PowerUp::Speed, block._position, ResourceManager::getTexture2D("powerup_speed"), glm::vec3(0.5, 0.5, 1.0), 0.0));
	}
	else if (shouldSpawn(change))
	{
		_powerups.emplace_back(PowerUp(PowerUp::Sticky, block._position, ResourceManager::getTexture2D("powerup_sticky"), glm::vec3(1.0, 0.5, 1.0), 20.0));
	}
	else if (shouldSpawn(change))
	{
		_powerups.emplace_back(PowerUp(PowerUp::PassThrough, block._position, ResourceManager::getTexture2D("powerup_passthrough"), glm::vec3(0.5, 1.0, 0.5), 10.0));
	}
	else if (shouldSpawn(change))
	{
		_powerups.emplace_back(PowerUp(PowerUp::PadSizeIncrease, block._position, ResourceManager::getTexture2D("powerup_increase"), glm::vec3(1.0, 0.6, 0.4), 0.0));
	}
	else if (shouldSpawn(change / 3))
	{
		_powerups.emplace_back(PowerUp(PowerUp::Confuse, block._position, ResourceManager::getTexture2D("powerup_confuse"), glm::vec3(1.0, 0.3, 0.3), 15.0));
	}
	else if (shouldSpawn(change / 3))
	{
		_powerups.emplace_back(PowerUp(PowerUp::Chaos, block._position, ResourceManager::getTexture2D("powerup_chaos"), glm::vec3(0.9, 0.25, 0.25), 15.0));
	}
}

void Game::updatePowerUps(GLfloat dt)
{
	auto isOtherPowerUpActive = [](std::vector<PowerUp>& powerUps, PowerUp::Type type) {
		for (const auto& it : powerUps)
		{
			if (it._activated && it._type == type)
			{
				return GL_TRUE;
			}
		}
		return GL_FALSE;
	};

	for (auto& it : _powerups)
	{
		it._position += it._velocity * dt;
		if (it._activated)
		{
			it._duration -= dt;
			if (it._duration < 0.0f)
			{
				it._activated = GL_FALSE;
				if (it._type == PowerUp::Sticky && !isOtherPowerUpActive(_powerups, PowerUp::Sticky))
				{
					_ball->_sticky = GL_FALSE;
					_player->_color = glm::vec3(1.0);
				}
				else if (it._type == PowerUp::PassThrough && !isOtherPowerUpActive(_powerups, PowerUp::PassThrough))
				{
					_ball->_passThrough = GL_FALSE;
					_player->_color = glm::vec3(1.0);
				}
				else if (it._type == PowerUp::Confuse && !isOtherPowerUpActive(_powerups, PowerUp::Confuse))
				{
					_effects->_confuse = GL_FALSE;
				}
				else if (it._type == PowerUp::Chaos && !isOtherPowerUpActive(_powerups, PowerUp::Chaos))
				{
					_effects->_chaos = GL_FALSE;
				}
			}
		}
	}

	_powerups.erase(std::remove_if(_powerups.begin(), _powerups.end(), [](const PowerUp& it) {
		return it._destroyed && !it._activated;
	}), _powerups.end());
}

void Game::activePowerup(const PowerUp& powerUp)
{
	if (powerUp._type == PowerUp::Speed)
	{
		_ball->_velocity *= 1.2;
	}
	else if (powerUp._type == PowerUp::Sticky)
	{
		_ball->_sticky = GL_TRUE;
		_player->_color = glm::vec3(1.0, 0.5, 1.0);
	}
	else if (powerUp._type == PowerUp::PassThrough)
	{
		_ball->_passThrough = GL_TRUE;
		_player->_color = glm::vec3(1.0, 0.5, 0.5);
	}
	else if (powerUp._type == PowerUp::PadSizeIncrease)
	{
		_player->_size.x += 50.0;
	}
	else if (powerUp._type == PowerUp::Confuse)
	{
		if (!_effects->_chaos)
		{
			_effects->_confuse = GL_TRUE;
		}
	}
	else if (powerUp._type == PowerUp::Chaos)
	{
		if (!_effects->_confuse)
		{
			_effects->_chaos = GL_TRUE;
		}
	}
}
