#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <GL//glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

// Render each of image
// Note:center is left-top
class SpriteRenderer
{
public:
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();

public:
	void drawSprite(Texture2D& texture2D, const glm::vec2& position, const glm::vec2& size = glm::vec2(10, 10), GLfloat rotate = 0.0f, const glm::vec3& color = glm::vec3(1.0f));

private:
	Shader& _shader;
	GLuint _quadVAO;

private:
	void initRenderData();
};
#endif
