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


#include "pch.hpp"
#include "shader.hpp"
#include "file.hpp"
#include "opengl.hpp"

namespace cs350 {

	/**************************************************************************
	*!
	\fn     shader::create

	\brief 
	Creates a shader

	\param  GLenum shaderType
	the shader type this will be

	\param  const char* filename
	the filename from which read the code and compile it

	*
	**************************************************************************/
	void shader::create(GLenum shaderType, const char* filename)
    {
		//setting the type
        mType = shaderType;

		//setting the filename
        mFilename = filename;

		//if the filename is empty return
		assert(filename != nullptr);

		//creating a shader with the given type
		mHandle = glCreateShader(mType);

		//string to store the parsed code
		std::string parsed;

		//calling to parse the code
		parseShader(filename, parsed);

		//storing it in a const char * because of openGL
		const char* sourceCode = parsed.c_str();

		//setting the source code
		glShaderSource(mHandle, 1, &sourceCode, NULL);

		//compiling the shader code
		glCompileShader(mHandle);

		//checking if it compiled properly
		GLint status;
		glGetShaderiv(mHandle, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;

			glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];
			glGetShaderInfoLog(mHandle, infoLogLength, NULL, strInfoLog);

			const char* strShaderType = NULL;
			switch (shaderType)
			{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
			}

			fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
			delete[] strInfoLog;
		}

    }

	/**************************************************************************
	*!
	\fn     shader::GetHandle

	\brief 
	getter function for the handle of the shader

	\return const GLuint
	The handle

	*
	**************************************************************************/
    const GLuint shader::GetHandle() const
    {
        return mHandle;
    }

	/**************************************************************************
	*!
	\fn     parseShader

	\brief 
	function to parse the code from a file to a string to compile it

	\param  const char* file
	the filename from which to read

	\param  std::string& container
	 string which will contain the code

	*
	**************************************************************************/
	void parseShader(const char* file, std::string& container)
	{
		//stream to read from the file
		std::ifstream inFile(file, std::ios::in);

		//checking if it was opened
		if (!inFile)
		{
			std::cout << "ERROR WHILE TRYING TO OPEN " << file << " FILE" << std::endl;

		}

		//reading the code from the file and adding it to the string
		while (inFile.good())
		{
			int c = inFile.get();
			if (!inFile.eof())
				container.push_back((char)c);
		}
		//closing the file
		inFile.close();
	}

	/**************************************************************************
	*!
	\fn     shaderProgram::create

	\brief 
	creates a shader program

	\param  std::string vertex
	 string containing the filename of the vertex shader

	\param  std::string fragment
	 string containing the filename of the fragment shader

	*
	**************************************************************************/
	void shaderProgram::create(std::string vertex, std::string fragment)   
	{
		//creating the vertex and fragment shaders
		mVertex.create(GL_VERTEX_SHADER, vertex.data());
		mFragment.create(GL_FRAGMENT_SHADER, fragment.data());

		//creating a shader program
		mHandle = glCreateProgram();

		//attaching the fragment shader
		glAttachShader(mHandle, mFragment.GetHandle());

		//attaching the vertex shader
		glAttachShader(mHandle, mVertex.GetHandle());

		//linking both shaders
		glLinkProgram(mHandle);

		//checking if it linked succesfully
		GLint status;
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar* strInfoLog = new GLchar[infoLogLength + 1];
			glGetProgramInfoLog(mHandle, infoLogLength, NULL, strInfoLog);
			fprintf(stderr, "Linker failure: %s\n", strInfoLog);
			delete[] strInfoLog;
		}

		//Detaching the fragment shader
		glDetachShader(mHandle, mFragment.GetHandle());

		//Detaching the vertex shader
		glDetachShader(mHandle, mVertex.GetHandle());
	}

	/**************************************************************************
	*!
	\fn     shaderProgram::GetHandle

	\brief 
	getter function for the handle of the shader program

	\return const GLuint
	the handle

	*
	**************************************************************************/
	const GLuint shaderProgram::GetHandle() const
	{
		return mHandle;
	}

	/**************************************************************************
	*!
	\fn     shaderProgram::use

	\brief 
	utility function to set that this is the shader that will be used

	*
	**************************************************************************/
	void shaderProgram::use()
	{
		// Bind the shader program and this object's VAO
		glUseProgram(mHandle);
	}
}
