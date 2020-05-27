#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>


GLuint TextureLoader::loadTexture(const std::string& textureFilePath)
{
	GLuint				newTexture = 0;
	unsigned char		*buffer;
	int t_width, t_height, t_nrChannels;

	stbi_set_flip_vertically_on_load(true);
	buffer = stbi_load(textureFilePath.c_str(), &t_width, &t_height, &t_nrChannels, 0);

	//Check if texture was loaded correctly
	if(!buffer)
	{
		std::size_t found = textureFilePath.find_last_of("/\\");
		std::cout << "Failed the load the texture: " << textureFilePath.substr(found + 1) << std::endl;
		return 0;
	}

	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);

	if (newTexture) {

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	if(t_nrChannels > 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);

	// Setup default texture properties
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(buffer);
	return newTexture;
}


GLuint TextureLoader::loadTexture(const std::string& textureFilePath, const TextureGenProperties& textureProperties)
{
	GLuint				newTexture = 0;
	unsigned char		*buffer;
	int t_width, t_height, t_nrChannels;

	stbi_set_flip_vertically_on_load(false);
	buffer = stbi_load(textureFilePath.c_str(), &t_width, &t_height, &t_nrChannels, 0);

	//Check if texture was loaded correctly
	if (!buffer)
	{
		std::size_t found = textureFilePath.find_last_of("/\\");
		std::cout << "Failed the load the texture: " << textureFilePath.substr(found + 1) << std::endl;
		return 0;
	}

	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, textureProperties.internalFormat, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);


	// Setup default texture properties
	if (newTexture) {

		// Verify we don't use GL_LINEAR_MIPMAP_LINEAR which has no meaning in non-mipmapped textures.  If not set, default to GL_LINEAR (bi-linear) filtering.
		GLint minFilter = (!textureProperties.genMipMaps && textureProperties.minFilter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR : textureProperties.minFilter;
		GLint maxFilter = (!textureProperties.genMipMaps && textureProperties.maxFilter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR : textureProperties.maxFilter;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureProperties.anisotropicLevel);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureProperties.wrap_s);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureProperties.wrap_t);

		if (textureProperties.genMipMaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	stbi_image_free(buffer);
	return newTexture;
}



GLuint TextureLoader::loadCubeMapTexture(
	const std::string& directory,
	const std::string& file_prefix,
	const std::string& file_extension,
	GLenum format,
	GLint minFilter,
	GLint maxFilter,
	const GLfloat anisotropicLevel,
	const GLint wrap_s,
	const GLint wrap_t,
	const GLint wrap_r,
	const bool genMipMaps) {

	GLuint		newTexture = 0;
	GLuint		w = 0;
	GLuint		h = 0;
	BYTE		*buffer;

	// 1. Setup texture names in array (makes following setup loop easier to implement)
	static const std::string cubeFaceName[6] = {

		std::string("_positive_x"),
		std::string("_negative_x"),
		std::string("_positive_y"),
		std::string("_negative_y"),
		std::string("_positive_z"),
		std::string("_negative_z")
	};

	// 2. Generate cubemap texture
	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, newTexture);

	// 3. Verify we don't use GL_LINEAR_MIPMAP_LINEAR which has no meaning in non-mipmapped textures.  If not set, default to GL_LINEAR (bi-linear) filtering
	minFilter = (!genMipMaps && minFilter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR : minFilter;
	maxFilter = (!genMipMaps && maxFilter == GL_LINEAR_MIPMAP_LINEAR) ? GL_LINEAR : maxFilter;

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, maxFilter);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropicLevel);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_r);


	// 4. Load bitmap images and setup each one in turn
	for (int i = 0; i < 6; i++) {

		// Generate filename
		std::string filename = directory;

		filename.append(file_prefix);
		filename.append(cubeFaceName[i]);
		filename.append(file_extension);

		int t_width, t_height, t_nrChannels;

		buffer = stbi_load(filename.c_str(), &t_width, &t_height, &t_nrChannels, 0);
		//Check if texture was loaded correctly
		if (!buffer)
		{
			std::size_t found = filename.find_last_of("/\\");
			std::cout << "Failed the load the texture: " << filename.substr(found + 1) << std::endl;
			return 0;
		}
		if (genMipMaps)
		{
			// Setup Mip-Mapped cubemap texture if required
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			// Setup single cubemap texture image
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	}
	stbi_image_free(buffer);
	return newTexture;
}
