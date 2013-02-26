/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include <map>

#include "cinder/gl/gl.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"

namespace cinder { namespace gl {

//! Represents an OpenGL GLSL program. \ImplShared
class GlslProg {
  public: 
	/** 
	 * Creates an empty GLSL program. 
	 */
	GlslProg() {}
	
	//! Creates a GLSL program from the input shader source files. 
	/** 
	 * Generates a compiled shader program using the input shader data sources by calling the necessary OpenGL routines.
	 * For each supplied shader parameter, a shader is created, loaded from source, compiled, and then linked. 
	 * 
	 * \param vertexShader the required data source for the vertex program
	 * \param fragmentShader the optional data source for the fragment program
	 * \param geometryShader the optional data source for the geometry program
	 * \param geometryInputType the GL primitive input type for the geometry shader
	 * \param geometryOutputType the GL primitive output type for the geometry shader
	 * \param geometryOutputVertices the number of output primitives generated from the geometry program
	 * 
	 * \throws GlslProgCompileExc If any shader program fails to compile
	 * 
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glCreateShader.xml">glCreateShader</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glShaderSource.xml">glShaderSource</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glCompileShader.xml">glCompileShader</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glGetShader.xml">glGetShaderiv</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glAttachShader.xml">glAttachShader</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glLinkProgram.xml">glLinkProgram</a>
	 */
	GlslProg( DataSourceRef vertexShader, DataSourceRef fragmentShader = DataSourceRef(), DataSourceRef geometryShader = DataSourceRef(), 
        GLint geometryInputType = GL_POINTS, GLint geometryOutputType = GL_TRIANGLES, GLint geometryOutputVertices = 0);
    
	//! Creates a GLSL program from raw shader input.
	/** 
	 * Generates a compiled shader program using the raw char arrays for each shader. For each supplied shader parameter, 
	 * a shader is created, loaded from source, compiled, and then linked. 
	 * 
	 * \param vertexShader the required char array representing the source for the vertex program
	 * \param fragmentShader the optional char array representing the source for the fragment program
	 * \param geometryShader the optional char array representing the source for the geometry program
	 * \param geometryInputType the GL primitive input type for the geometry shader
	 * \param geometryOutputType the GL primitive output type for the geometry shader
	 * \param geometryOutputVertices the number of output primitives generated from the geometry program
	 * 
	 * \throws GlslProgCompileExc If any shader program fails to compile
	 * 
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glCreateShader.xml">glCreateShader</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glShaderSource.xml">glShaderSource</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glCompileShader.xml">glCompileShader</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glGetShader.xml">glGetShaderiv</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glAttachShader.xml">glAttachShader</a>
	 * \see <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glLinkProgram.xml">glLinkProgram</a>
	 */
	GlslProg( const char *vertexShader, const char *fragmentShader = 0, const char *geometryShader = 0, GLint geometryInputType = GL_POINTS, GLint geometryOutputType = GL_TRIANGLES, GLint geometryOutputVertices = 0);

	//! Binds the current GLSL program using <a href="http://www.opengl.org/sdk/docs/man/xhtml/glUseProgram.xml">glUseProgram</a>
	void			bind() const;
	//! Un-binds the current GLSL program.
	static void		unbind();

	//! Returns the GLSL program handle
	GLuint			getHandle() const { return mObj->mHandle; }

	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform1i
	void	uniform( const std::string &name, int data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform2i
	void	uniform( const std::string &name, const Vec2i &data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform1iv
	void	uniform( const std::string &name, const int *data, int count );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform2iv	
	void	uniform( const std::string &name, const Vec2i *data, int count );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform1f
	void	uniform( const std::string &name, float data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform2f
	void	uniform( const std::string &name, const Vec2f &data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform3f
	void	uniform( const std::string &name, const Vec3f &data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform4f
	void	uniform( const std::string &name, const Vec4f &data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform3f
	void	uniform( const std::string &name, const Color &data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform4f
	void	uniform( const std::string &name, const ColorA &data );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniformMatrix2fv
	void	uniform( const std::string &name, const Matrix22f &data, bool transpose = false );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniformMatrix3fv
	void	uniform( const std::string &name, const Matrix33f &data, bool transpose = false );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniformMatrix4fv
	void	uniform( const std::string &name, const Matrix44f &data, bool transpose = false );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform1fv
	void	uniform( const std::string &name, const float *data, int count );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform2fv
	void	uniform( const std::string &name, const Vec2f *data, int count );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform3fv
	void	uniform( const std::string &name, const Vec3f *data, int count );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniform4fv
	void	uniform( const std::string &name, const Vec4f *data, int count );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniformMatrix2fv
	void	uniform( const std::string &name, const Matrix22f *data, int count, bool transpose = false );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniformMatrix3fv
	void	uniform( const std::string &name, const Matrix33f *data, int count, bool transpose = false );
	//! binds a uniform value to the GLSL program using glGetUniformLocation with the input name parameter and glUniformMatrix4fv
	void	uniform( const std::string &name, const Matrix44f *data, int count, bool transpose = false );

	/**
	 * Returns the uniform location within the GLSL program for the given name parameter by using glGetUniformLocation.
	 * Used for binding input values from the GlslProg uniform methods. 
	 * 
	 * \param name the string name identifying the program uniform
	 * \return the location integer or -1 if no such location exists
	 * 
	 * \see <a href="http://www.opengl.org/sdk/docs/man/xhtml/glGetUniformLocation.xml">glGetUniformLocation</a>
	 */
	GLint	getUniformLocation( const std::string &name );
	/**
	 * Returns the attribute location within the GLSL program for the given name parameter by using glGetAttribLocation.
	 * 
	 * \param name the string name identifying the program attribute
	 * \return the location integer or -1 if no such location exists
	 * 
	 * \see <a href="http://www.opengl.org/sdk/docs/man/xhtml/glGetAttribLocation.xml">glGetAttribLocation</a>
	 */
	GLint	getAttribLocation( const std::string &name );

	/**
	 * Returns the information log for the specified shader object formatted as a string.
	 * 
	 * \see GlslProg::getHandle
	 * \see <a href="http://www.opengl.org/sdk/docs/man/xhtml/glGetShaderInfoLog.xml">glGetShaderInfoLog</a>
	 */
	std::string		getShaderLog( GLuint handle ) const;

  protected:
	void			loadShader( Buffer shaderSourceBuffer, GLint shaderType );
	void			loadShader( const char *shaderSource, GLint shaderType );
	void			attachShaders();
	void			link();

	struct Obj {
		Obj() : mHandle( 0 ) {}
		~Obj();
		
		GLuint						mHandle;
		std::map<std::string,int>	mUniformLocs;
	};
 
	std::shared_ptr<Obj>	mObj;

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> GlslProg::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &GlslProg::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

class GlslProgCompileExc : public std::exception {
 public:	
	GlslProgCompileExc( const std::string &log, GLint aShaderType ) throw();
	virtual const char* what() const throw()
	{
		return mMessage;
	}

 private:
	char	mMessage[16001];
	GLint	mShaderType;
};

class GlslNullProgramExc : public std::exception {
 public:	
	virtual const char* what() const throw()
	{
		return "Glsl: Attempt to use null shader";
	}

};

} } // namespace cinder::gl
