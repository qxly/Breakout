#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
};

#endif
