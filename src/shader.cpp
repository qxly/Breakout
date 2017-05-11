#include "shader.h"

#include <iostream>

Shader& Shader::use()
{
	glUseProgram(_id);
	return *this;
}

void Shader::compile(const GLchar* vertex_source, const GLchar* fragment_source, const GLchar* geometry_source /*= nullptr*/)
{
	GLuint vertex_id, fragment_id, geometry_id;
	
	// Vertex Shader
	vertex_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_id, 1, &vertex_source, NULL);
	glCompileShader(vertex_id);
	checkCompileErrors(vertex_id, Compile::kVertex);

	// Fragment Shader
	fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_id, 1, &fragment_source, NULL);
	glCompileShader(fragment_id);
	checkCompileErrors(fragment_id, Compile::kFragment);

	// Geometry Shader
	if (nullptr != geometry_source)
	{
		geometry_id = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry_id, 1, &geometry_source, NULL);
		glCompileShader(geometry_id);
		checkCompileErrors(geometry_id, Compile::kGeometry);
	}

	// Shader Program
	_id = glCreateProgram();
	glAttachShader(_id, vertex_id);
	glAttachShader(_id, fragment_id);
	if (nullptr != geometry_source)
	{
		glAttachShader(_id, geometry_id);
	}
	glLinkProgram(_id);
	checkCompileErrors(_id, Compile::kProgram);

	// clean
	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
	if (nullptr != geometry_source)
	{
		glDeleteShader(geometry_id);
	}
}

void Shader::setFloat(const GLchar* name, GLfloat value, GLboolean use_shader /*= false*/)
{
	if (use_shader)
	{
		use();
	}
	glUniform1f(glGetUniformLocation(_id, name), value);
}

void Shader::setInteger(const GLchar *name, GLint value, GLboolean use_shader /*= false*/)
{
	if (use_shader)
	{
		use();
	}
	glUniform1i(glGetUniformLocation(_id, name), value);
}

void Shader::setVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean use_shader /*= false*/)
{
	if (use_shader)
	{
		use();
	}
	glUniform2f(glGetUniformLocation(_id, name), x, y);
}

void Shader::setVector2f(const GLchar* name, const glm::vec2& value, GLboolean use_shader /*= false*/)
{
	setVector2f(name, value.x, value.y, use_shader);
}

void Shader::setVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader /*= false*/)
{
	if (use_shader)
	{
		use();
	}
	glUniform3f(glGetUniformLocation(_id, name), x, y, z);
}

void Shader::setVector3f(const GLchar* name, const glm::vec3& value, GLboolean use_shader /*= false*/)
{
	setVector3f(name, value.x, value.y, value.z, use_shader);
}

void Shader::setVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean use_shader /*= false*/)
{
	if (use_shader)
	{
		use();
	}
	glUniform4f(glGetUniformLocation(_id, name), x, y, z, w);
}

void Shader::setVector4f(const GLchar* name, const glm::vec4& value, GLboolean use_shader /*= false*/)
{
	setVector4f(name, value.x, value.y, value.z, value.w, use_shader);
}

void Shader::setMatrix4(const GLchar* name, const glm::mat4& matrix, GLboolean use_shader /*= false*/)
{
	if (use_shader)
	{
		use();
	}
	glUniformMatrix4fv(glGetUniformLocation(_id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(GLuint object, Compile type)
{
	GLint success_id;
	GLchar info_log[1024];
	if (Compile::kProgram != type)
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success_id);
		if (!success_id)
		{
			glGetShaderInfoLog(object, 1024, NULL, info_log);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< info_log << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success_id);
		if (!success_id)
		{
			glGetProgramInfoLog(object, 1024, NULL, info_log);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< info_log << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}
