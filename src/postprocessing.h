#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "texture.h"

class Shader;

class PostProcessing {
public:
	PostProcessing(Shader& shader, GLuint width, GLuint height);

private:
	Shader& _postShader;
	GLuint _width;
	GLuint _height;

	Texture2D _texture2D;

public:
	GLboolean _shake, _confuse, _chaos;

public:
	void beginRender();
	void endRender();
	void render(GLfloat time);

private:
	GLuint _msfbo, _fbo;
	GLuint _rbo;
	GLuint _vao;

	void initRenderData();
};
#endif
