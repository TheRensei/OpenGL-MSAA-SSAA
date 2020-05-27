//  ShaderLoader.h - Methods to load and compile OpenGL shaders
#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <string>
#include <glad/glad.h>

// Declare GLSL setup return / error codes
typedef enum GLSL_ERROR_CODES {

	GLSL_OK = 0,

	GLSL_VERTEX_SHADER_REQUIRED_ERROR, // returned if a geometry shader but no vertex shader is defined

	GLSL_SHADER_SOURCE_NOT_FOUND, // shader source string not found
	GLSL_SHADER_OBJECT_CREATION_ERROR, // shader object cannot be created by OpenGL
	GLSL_SHADER_COMPILE_ERROR, // shader object could not be compiled

	GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND,
	GLSL_TESSELLATION_CONTROL_SHADER_SOURCE_NOT_FOUND,
	GLSL_TESSELLATION_EVALUATION_SHADER_SOURCE_NOT_FOUND,
	GLSL_GEOMETRY_SHADER_SOURCE_NOT_FOUND,
	GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND,

	GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR,
	GLSL_TESSELLATION_CONTROL_SHADER_OBJECT_CREATION_ERROR,
	GLSL_TESSELLATION_EVALUATION_SHADER_OBJECT_CREATION_ERROR,
	GLSL_GEOMETRY_SHADER_OBJECT_CREATION_ERROR,
	GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR,

	GLSL_VERTEX_SHADER_COMPILE_ERROR,
	GLSL_TESSELLATION_CONTROL_SHADER_COMPILE_ERROR,
	GLSL_TESSELLATION_EVALUATION_SHADER_COMPILE_ERROR,
	GLSL_GEOMETRY_SHADER_COMPILE_ERROR,
	GLSL_FRAGMENT_SHADER_COMPILE_ERROR,

	GLSL_PROGRAM_OBJECT_CREATION_ERROR,
	GLSL_PROGRAM_OBJECT_LINK_ERROR

} GLSL_ERROR;


class ShaderLoader{

public:

	// Basic shader object creation function takes a path to a vertex shader file and fragment shader file and returns a bound and linked shader program object in *result.  No attribute bindings are specified in this function so it is assumed 'in' variable declarations have associated location declarations in the shader source file
	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& fsPath, GLuint *result);

	// Overload of createShaderProgram that allows vertex attribute locations to be declared in C/C++ and bound during shader creation.  This avoids the need to specify the layout in the vertex shader 'in' variable declarations, but this is not as convinient!
	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& fsPath, GLuint *result, int numAttributes, ...);

	// Overloaded version of createShaderProgram that takes a geometry shader filename as well as a vertex and fragment shader filename
	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& gsPath, const std::string& fsPath, GLuint *result);

	// Overloaded version of createShaderProgram that takes a tessellation control and evaluation shader as well as a vertex and fragment shader filename
	static GLSL_ERROR createShaderProgram(const std::string& vsPath, const std::string& tcsPath, const std::string& tesPath, const std::string& fsPath, GLuint *result);
};

#endif