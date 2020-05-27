#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "ShaderLoader.h"
#include "TextureLoader.h"



// Holds all state information relevant to a character as loaded using FreeType
struct Character 
{
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class TextRenderer {
private:
	GLuint	VAO;
	GLuint	VBO;

	GLuint	textShader;
	GLuint	texture;
	std::map<GLchar, Character> Characters;

	void setupVAO();
	void loadFonts();
	void loadShader();


public:
	TextRenderer(int Width, int Heigh);
	~TextRenderer();

	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};

#endif