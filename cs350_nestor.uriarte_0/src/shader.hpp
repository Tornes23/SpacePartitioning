/*!**************************************************************************
\file    shader.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 13:08:16 2020

\brief	 This file containsm the implementation of the
shader and shaderProgram classes

The functions included are:
- void shader::create(GLenum shaderType, const char* filename);
- const unsigned int shader::GetHandle() const;
- void parseShader(const char* file, std::string& container);
- void shaderProgram::create(std::string vertex, std::string fragment);
- const GLuint shaderProgram::GetHandle() const;
- void shaderProgram::use();

***************************************************************************/

#pragma once
#include "math.hpp"
#include "opengl.hpp"

namespace cs350 {

	void parseShader(const char* file, std::string& container);

    class shader
    {
	public:
		void create(GLenum shaderType, const char* filename);
		const GLuint GetHandle() const;

	private:
		std::string mFilename;
		GLenum mType;
		GLuint mHandle{};
       
    };

	class shaderProgram
	{
	public:
		void create(std::string vertex, std::string fragment);

		const GLuint GetHandle() const;

		void use();

	private:
		shader mFragment{};
		shader mVertex{};

		GLuint mHandle{};
	};
}