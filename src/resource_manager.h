#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <GL/glew.h>

#include "texture.h"
#include "shader.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders.
class ResourceManager {
private:
	ResourceManager() {}

private:
	static std::map<std::string, Shader> _shaders;
	static std::map<std::string, Texture2D> _texture2Ds;

public:
	static Shader& loadShader(const GLchar* v_shader_file, const GLchar* f_shader_file, const GLchar* g_shader_file, const std::string& name);
	static Shader& getShader(const std::string& name);

	static Texture2D& loadTexture2D(const GLchar* file, GLboolean alpha, const std::string& name);
	static Texture2D& getTexture2D(const std::string& name);

	static void clear();

private:
	static Shader loadShaderFromFile(const GLchar* v_shader_file, const GLchar* f_shader_file, const GLchar* g_shader_file = nullptr);
	static Texture2D loadTexture2DFromFile(const GLchar* file, GLboolean alpha);
};

#endif
