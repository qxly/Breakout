#include "sprite_renderer.h"

SpriteRenderer::SpriteRenderer(Shader& shader) : _shader(shader)
{
	this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &this->_quadVAO);
}

void SpriteRenderer::drawSprite(const Texture2D& texture2D, const glm::vec2& position, const glm::vec2& size /*= glm::vec2(10, 10)*/, GLfloat rotate /*= 0.0f*/, const glm::vec3& color /*= glm::vec3(1.0f)*/)
{
	_shader.use();

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0));

	model = glm::translate(model, glm::vec3(0.5 * size.x, 0.5 * size.y, 0.0));
	model = glm::rotate(model, rotate, glm::vec3(0.0, 0.0, 1.0));
	model = glm::translate(model, glm::vec3(-0.5 * size.x, -0.5 * size.y, 0.0));

	model = glm::scale(model, glm::vec3(size, 1.0));

	_shader.setMatrix4("model", model);
	_shader.setVector3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture2D.bind();

	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
	GLuint vbo;
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &_quadVAO);
	glBindVertexArray(_quadVAO);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
