#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Hold shader
class Shader {
public:
	Shader() {}

	GLuint _id;	

public:
	Shader& use();

	void compile(const GLchar* vertex_source, const GLchar* fragment_source, const GLchar* geometry_source = nullptr);

	void setFloat(const GLchar* name, GLfloat value, GLboolean use_shader = false);
	void setInteger(const GLchar *name, GLint value, GLboolean use_shader = false);
	void setVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean use_shader = false);
	void setVector2f(const GLchar* name, const glm::vec2& value, GLboolean use_shader = false);
	void setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader = false);
	void setVector3f(const GLchar* name, const glm::vec3& value, GLboolean use_shader = false);
	void setVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean use_shader = false);
	void setVector4f(const GLchar* name, const glm::vec4& value, GLboolean use_shader = false);
	void setMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean use_shader = false);

private:
	enum Compile : uint8_t
	{
		kVertex,
		kFragment,
		kGeometry,
		kProgram
	};
	void checkCompileErrors(GLuint object, Compile type);
};

#endif
