// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
//#include "stable.h"
#include "Shader.h"
//#include <vector>
//#include <fstream>
#include <sstream>
//#include <iostream>




Shader::Shader()
	: mShaderProgram(0)
	, mVertexShader(0)
	, mFragShader(0)
{
	
}

Shader::~Shader()
{

}

/*
program=glCreateProgram();
//glAttachShader(program​, vshader​);
//char *vs = NULL,*fs = NULL,*fs2 = NULL;
//vshader = glCreateShader(GL_VERTEX_SHADER);
//fshader = glCreateShader(GL_FRAGMENT_SHADER);

shader->CompileShader("./Basic.vert", GL_VERTEX_SHADER, vshader);
shader->CompileShader("./Basic.frag", GL_FRAGMENT_SHADER, fshader);

//const char * vv = vs;
//const char * ff = fs;

//glShaderSource(vshader, 1, &vv,NULL);
//glShaderSource(fshader, 1, &ff,NULL);

//glCompileShader(vshader);
//glCompileShader(fshader);


glAttachShader(program,vshader);
glAttachShader(program,fshader);

glLinkProgram(program);
glUseProgram(program);
*/

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
	// Compile vertex and pixel shaders
    if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
        !CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
	{
        printf("Shader load() fail ........\n");
		return false;
	}
	
	// Now create a shader program that
	// links together the vertex/frag shaders
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);
	
	// Verify that the program linked successfully
	if (!IsValidProgram())
	{
		return false;
	}
	

    printf("Shader load() ........\n");//, fileName.c_str());
	return true;
}

void Shader::Unload()
{
	// Delete the program/shaders
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

void Shader::SetActive()
{
	// Set this program as the active one
	glUseProgram(mShaderProgram);
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// Find the uniform by this name
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// Send the matrix data to the uniform
	glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.GetAsFloatPtr());
}

bool Shader::CompileShader(const std::string& fileName,
	GLenum shaderType,
	GLuint& outShader)
{
	// Open file
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		// Read all the text into a string
		std::stringstream sstream;
		sstream << shaderFile.rdbuf();
		std::string contents = sstream.str();
        //std::cout << contents;
		const char* contentsChar = contents.c_str();
		
		// Create a shader of the specified type
		outShader = glCreateShader(shaderType);
		// Set the source characters and try to compile
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		glCompileShader(outShader);
		
		if (!IsCompiled(outShader))
		{
            GLint maxLength = 0;
            glGetShaderiv(outShader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(outShader, maxLength, &maxLength, &errorLog[0]);

            // Provide the infolog in whatever manor you deem best.
            // Exit with failure.
            glDeleteShader(outShader); // Don't leak the shader.

            printf("ERROR:  %s\n", &errorLog[0]);

            printf("Failed to compile shader %s\n", fileName.c_str());

			return false;
		}
	}
	else
	{
        printf("Shader can't open : %s\n", fileName.c_str());
		return false;
	}
	
	return true;
}

bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	// Query the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
        //SDL_Log("GLSL Compile Failed:\n%s", buffer);
		return false;
	}
	
	return true;
}

bool Shader::IsValidProgram()
{
	
	GLint status;
	// Query the link status
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
        //SDL_Log("GLSL Link Status:\n%s", buffer);
		return false;
	}	
	return true;
}
