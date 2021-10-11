#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class vertFragShader
{
	//shader class that builds a program out of vert and frag shader code
	//-------------------------------------------------------------------
public:
	// Program ID
	unsigned int ID;

	// contructor for reading and building shader
	vertFragShader(const char* vertexPath, const char* fragmentPath)
	{
		// get source code from file paths
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		// ensure ifstream objects throw exceptions
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{   
			// open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read files into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close files
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_READING_FAILED" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// print compile errors
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// print compile errors
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		// print linking errors if any
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	};

	// use shader
	void use()
	{
		glUseProgram(ID);
	};

	// utility functions
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	};
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setVec4(const std::string &name, float val1, float val2, float val3, float val4) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), val1, val2, val3, val4);
	};
};

class computeShader
{
	//shader class that builds a program out of compute shader code
	//-------------------------------------------------------------------
public:
	// Program ID
	unsigned int ID;

	// contructor for reading and building shader
	computeShader(const char* computePath)
	{
		// get source code from file paths
		std::string computeCode;
		std::ifstream cShaderFile;
		// ensure ifstream objects throw exceptions
		cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{   
			// open file
			cShaderFile.open(computePath);
			std::stringstream cShaderStream;
			// read file into streams
			cShaderStream << cShaderFile.rdbuf();
			// close file
			cShaderFile.close();
			// convert stream into string
			computeCode = cShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_READING_FAILED" << std::endl;
		}
		const char* cShaderCode = computeCode.c_str();

		// compile shaders
		unsigned int compute;
		int success;
		char infoLog[512];

		// vertex shader
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &cShaderCode, NULL);
		glCompileShader(compute);
		// print compile errors
		glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(compute, 512, NULL, infoLog);
			std::cout<<"ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, compute);
		glLinkProgram(ID);
		// print linking errors if any
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(compute);
	};

	// use shader
	void use()
	{
		glUseProgram(ID);
	};

	void dispatch(int width, int height)
	{
		glDispatchCompute(width, height, 1);
	}

	// utility functions
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	};
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	};
	void setVec4(const std::string &name, float val1, float val2, float val3, float val4) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), val1, val2, val3, val4);
	};
};
#endif