// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
//#include "stable.h"
#include "Shader.h"
#include <sstream>


GLuint d_installed_program=-1; // global current installed program

Shader::Shader()
    : mProgram(0)
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
#include <map>
std::map<std::string, GLuint> cached_shader_id;

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
    std::string _key = vertName+fragName;

   /* auto search = cached_shader_id.find(_key);
    if ( search != cached_shader_id.end())
    {
        mProgram=search->second;
        std::cout << "DEBUG: Found " << search->first <<", id:" << mProgram << '\n';
    }
    else */
    {

        // Now create a shader program that
        // links together the vertex/frag shaders
        if (mProgram!=0)
        {
            printf("Error : Shader mProgram is not NULL!!\n");
            assert(false);
            return false;
        }
        mProgram = glCreateProgram(); /// leak 4
        if (mProgram==0)
        {
            printf("Error : Shader mProgram is  NULL!!\n");
            assert(false);
            return false;
        }
        // glDeleteProgram(mProgram); // works! clear leak!!!

      // Compile vertex and pixel shaders
         if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader))
        {
            printf("Error : Shader load() fail ........\n");
            assert(false);
            return false;
        }

        if (!CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
        {
            printf("Error : Shader load() fail ........\n");
            assert(false);
            return false;
        }
        //std::cout << "shader = " << mFragShader << "\n";
        glAttachShader(mProgram, mVertexShader);
        glAttachShader(mProgram, mFragShader); //
        glLinkProgram(mProgram);

        // Verify that the program linked successfully
        /*if (!IsValidProgram())
        {
            printf("ERROR: shader.load() \n");
            assert(false);
            return false;
        }*/
        // Textures.insert({aimaterial->GetName().C_Str(), _tex->d_tex_glname}); // c++11
        ///cached_shader_id.insert({_key, mProgram}); // ok
        glDetachShader(mProgram, mVertexShader); //
        glDetachShader(mProgram, mFragShader); //
        glDeleteShader(mVertexShader); //
        glDeleteShader(mFragShader);
   }
   printf("Shader load() =====> program = %d \n", mProgram);
   return true;
}

void Shader::Unload()
{
	// Delete the program/shaders
    glDeleteProgram(mProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);

}

void Shader::SetActive()
{
    glUseProgram(mProgram);
	// Set this program as the active one
    if (d_installed_program != mProgram)
    {
        //printf("new progrma =%d \n",mProgram);
        glUseProgram(mProgram);
        d_installed_program = mProgram;
    }
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// Find the uniform by this name
    GLuint loc = glGetUniformLocation(mProgram, name);
	// Send the matrix data to the uniform
	glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.GetAsFloatPtr());
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
    //  CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader))
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
        outShader = glCreateShader(shaderType);  // leak fixed!
        std::cout << "created shader = " << outShader << "\n";
		// Set the source characters and try to compile
        glShaderSource(outShader, 1, &(contentsChar), nullptr);
        glCompileShader(outShader); /// leak???
        GLint status;
        glGetShaderiv(outShader, GL_COMPILE_STATUS, &status);
        //if (IsCompiled(outShader)==false) // leak
        if (status != GL_TRUE)
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
        printf("GLSL Compile Failed:\n%s", buffer);
		return false;
	}	
	return true;
}

bool Shader::IsValidProgram()
{
	
	GLint status;
	// Query the link status
    glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
        glGetProgramInfoLog(mProgram, 511, nullptr, buffer);
        printf("ERROR: GLSL Link Status:\n%s", buffer);
		return false;
	}	
	return true;
}
