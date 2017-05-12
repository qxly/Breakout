#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <SOIL.h>

std::map<std::string, Shader> ResourceManager::_shaders;
std::map<std::string, Texture2D> ResourceManager::_texture2Ds;

Shader& ResourceManager::loadShader(const GLchar* v_shader_file, const GLchar* f_shader_file, const GLchar* g_shader_file, const std::string& name)
{
	_shaders[name] = loadShaderFromFile(v_shader_file, f_shader_file, g_shader_file);
	
	return _shaders[name];
}

Shader& ResourceManager::getShader(const std::string& name)
{
	return _shaders[name];
}

Texture2D& ResourceManager::loadTexture2D(const GLchar* file, GLboolean alpha, const std::string& name)
{
	_texture2Ds[name] = loadTexture2DFromFile(file, alpha);

	return _texture2Ds[name];
}

Texture2D& ResourceManager::getTexture2D(const std::string& name)
{
	return _texture2Ds[name];
}

void ResourceManager::clear()
{
	for (auto iter : _shaders)
	{
		glDeleteProgram(iter.second._id);
	}
	_shaders.clear();
	for (auto iter : _texture2Ds)
	{
		glDeleteTextures(1, &iter.second._id);
	}
	_texture2Ds.clear();
}

Shader ResourceManager::loadShaderFromFile(const GLchar* v_shader_file, const GLchar* f_shader_file, const GLchar* g_shader_file /*= nullptr*/)
{
	std::string vertex_code, fragment_code, geometry_code;
	
	try
	{
		std::ifstream vertex_shader_file(v_shader_file);
		std::ifstream fragment_shader_file(f_shader_file);

		std::stringstream v_shader_stream, f_shader_stream;
		v_shader_stream << vertex_shader_file.rdbuf();
		f_shader_stream << fragment_shader_file.rdbuf();

		vertex_shader_file.close();
		fragment_shader_file.close();

		vertex_code = v_shader_stream.str();
		fragment_code = f_shader_stream.str();

		if (nullptr != g_shader_file)
		{
			std::ifstream geometry_shader_file(g_shader_file);
			std::stringstream g_shader_stream;
			g_shader_stream << geometry_shader_file.rdbuf();
			geometry_shader_file.close();
			geometry_code = g_shader_stream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}

	const GLchar* v_shader_code = vertex_code.c_str();
	const GLchar* f_shader_code = fragment_code.c_str();
	const GLchar* g_shader_code = geometry_code.c_str();

	Shader shader;
	shader.compile(v_shader_code, f_shader_code, (g_shader_file != nullptr ? g_shader_code : nullptr));
	
	return shader;
}

Texture2D ResourceManager::loadTexture2DFromFile(const GLchar* file, GLboolean alpha)
{
	Texture2D texture2D;

	if (alpha)
	{
		texture2D._internal_format = GL_RGBA;
		texture2D._image_format = GL_RGBA;
	}

	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, texture2D._image_format == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

	texture2D.generate(width, height, image);
	SOIL_free_image_data(image);
	
	return texture2D;
}

