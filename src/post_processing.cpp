#include "post_processing.h"
#include "shader.h"
#include <iostream>

PostProcessing::PostProcessing(Shader& shader, GLuint width, GLuint height) : _postShader(shader), _width(width), _height(height)
{
	glGenFramebuffers(1, &_msfbo);
	glGenFramebuffers(1, &_fbo);
	glGenRenderbuffers(1, &_rbo);

	// _msfbo
	glBindFramebuffer(GL_FRAMEBUFFER, _msfbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, _width, _height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
	}

	// _fbo
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	_texture2D.generate(_width, _height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture2D._id, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render data
	initRenderData();

	_postShader.setInteger("image", 0, GL_TRUE);

	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset },  // top-left
		{ 0.0f,    offset },  // top-center
		{ offset,  offset },  // top-right
		{ -offset,  0.0f },  // center-left
		{ 0.0f,    0.0f },  // center-center
		{ offset,  0.0f },  // center - right
		{ -offset, -offset },  // bottom-left
		{ 0.0f,   -offset },  // bottom-center
		{ offset, -offset }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(_postShader._id, "offsets"), 9, (GLfloat*)offsets);

	GLint edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(_postShader._id, "edge_kernel"), 9, edge_kernel);

	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(_postShader._id, "blur_kernel"), 9, blur_kernel);
}

void PostProcessing::beginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _msfbo);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessing::endRender()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _msfbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
	glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::render(GLfloat time)
{
	_postShader.use();
	_postShader.setFloat("time", time);
	_postShader.setInteger("shake", _shake);
	_postShader.setInteger("confuse", _confuse);
	_postShader.setInteger("chaos", _chaos);

	glActiveTexture(GL_TEXTURE0);
	_texture2D.bind();
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessing::initRenderData()
{
	GLuint vbo;
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
