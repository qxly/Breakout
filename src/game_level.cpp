#include "game_level.h"

#include <fstream>
#include <sstream>

GameLevel::GameLevel()
{
}

GameLevel::~GameLevel()
{
}

void GameLevel::load(const GLchar* file, GLuint levelWidth, GLuint levelHeight)
{
	_file = file;
	_levelWidth = levelWidth;
	_levelHeight = levelHeight;

	// clear old data
	_bricks.clear();

	// load from file
	GLuint tilecode;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<GLuint>> tiledata;
	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tilecode)
			{
				row.push_back(tilecode);
			}
			tiledata.push_back(row);
		}
		if (!tiledata.empty())
		{
			init(tiledata, levelWidth, levelHeight);
		}
	}
}

void GameLevel::reset()
{
	load(_file.c_str(), _levelWidth, _levelHeight);
}

void GameLevel::draw(SpriteRenderer& renderer)
{
	for (auto& tile : _bricks)
	{
		if (!tile._destroyed)
		{
			tile.draw(renderer);
		}
	}
}

GLboolean GameLevel::isCompleted()
{
	for (const auto& tile : _bricks)
	{
		if (!tile._isSoild && !tile._destroyed)
		{
			return GL_FALSE;
		}
	}
	return GL_TRUE;
}

void GameLevel::init(const std::vector<std::vector<GLuint>>& tileData, GLuint levelWidth, GLuint levelHeight)
{
	// calculate dimensions
	GLuint height = tileData.size();
	GLuint width = tileData[0].size();
	GLfloat unit_width = levelWidth / static_cast<GLfloat>(width);
	GLfloat unit_height = levelHeight / static_cast<GLfloat>(height);

	for (GLuint i = 0; i < height; i++)
	{
		for (GLuint j = 0; j < width; j++)
		{
			if (tileData[i][j] == 1) // solid
			{
				glm::vec2 pos(unit_width * j, unit_height * i);
				glm::vec2 size(unit_width, unit_height);
				GameObject obj(pos, size, ResourceManager::getTexture2D("block_solid"), glm::vec3(0.8, 0.8, 0.8));
				obj._isSoild = GL_TRUE;
				_bricks.push_back(obj);
			}
			else if (tileData[i][j] > 1)
			{
				glm::vec2 pos(unit_width * j, unit_height * i);
				glm::vec2 size(unit_width, unit_height);
				glm::vec3 color = glm::vec3(1.0);
				if (tileData[i][j] == 2)
				{
					color = glm::vec3(0.2, 0.6, 1.0);
				}
				else if(tileData[i][j] == 3)
				{
					color = glm::vec3(0.0, 0.7, 0.0);
				}
				else if (tileData[i][j] == 4)
				{
					color = glm::vec3(0.8, 0.8, 0.4);
				}
				else if (tileData[i][j] == 5)
				{
					color = glm::vec3(1.0, 0.5, 0.0);
				}
				GameObject obj(pos, size, ResourceManager::getTexture2D("block"), color);
				_bricks.push_back(obj);
			} // other empty
		}
	}
}
