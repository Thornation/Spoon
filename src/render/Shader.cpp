#include "Shader.h"

#include <iostream>
#include <fstream>

std::string shader_path = "data/shaders/";

Shader::Shader(ShaderType type)
{
	this->type = type;

	if (type == Fragment)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else if (type == Vertex)
		shader = glCreateShader(GL_VERTEX_SHADER);
}

Shader::~Shader()
{

}

void Shader::LoadShader(std::string const &name)
{
	std::string ext;
	
	if (type == Fragment)
		ext = ".glfs";
	else if (type == Vertex)
		ext = ".glvs";

	std::string filepath = shader_path + name + ext;

	std::ifstream file(filepath);
	if (!file.is_open())
	{
		std::cout << "Warning: Shader::LoadShader: Failed to load shader " << filepath << std::endl;
		return;
	}

	std::string buffer = "";

	std::string line;
	while (getline(file, line))
		buffer += line + "\n";

	file.close();

	/* Create shader object */
	const char *shader_src = buffer.c_str();

	glShaderSource(shader, 1, &shader_src, NULL); // Set shader source
	glCompileShader(shader);

	/* Check for shader compilation errors */
	GLint sshader_compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &sshader_compiled);
	if (sshader_compiled != GL_TRUE)
	{
		std::cout << "Unable to compile vertex shader: " << shader << std::endl;
		PrintLog();
	}
}

void Shader::PrintLog()
{
	if (!glIsShader(shader))
	{
		std::cout << "Warning: OpenGL Shader " << shader << " is not a shader" << std::endl;
		return;
	}

	int max_length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length); // Get info string length

	/* Get shader log */
	char *info_log = new char[max_length];
	int info_log_length = 0;
	glGetShaderInfoLog(shader, max_length, &info_log_length, info_log);
	if (info_log_length > 0)
		std::cout << "OpenGL Shader Log:" << std::endl << info_log << std::endl;

	delete[] info_log;
}
