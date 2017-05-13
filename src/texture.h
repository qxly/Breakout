#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

// Hold Texture2D
class Texture2D {
public:
	Texture2D();

public:
	GLuint _id;
	GLuint _width = 0, _height = 0; // Width and height of loaded image in pixels
	GLuint _internal_format = GL_RGB; // Format of texture object
	GLuint _image_format = GL_RGB; // Format of loaded image

	GLuint _wrap_s = GL_REPEAT; // Wrapping mode on S axis
	GLuint _wrap_t = GL_REPEAT; // Wrapping mode on T axis
	GLuint _filter_min = GL_LINEAR; // Filtering mode if texture pixels < screen pixels
	GLuint _filter_max = GL_LINEAR; // Filtering mode if texture pixels > screen pixels

public:
	void generate(GLuint width, GLuint height, unsigned char* data);

	void bind() const;

};

#endif
