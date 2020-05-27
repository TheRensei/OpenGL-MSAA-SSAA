#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "TextureLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <fstream>
#include <iostream>

using namespace std;

/*Vertex Shader layouts:
 vertex data			- layout (location=0) in vec3
 normal data			- layout (location=1) in vec3
 texture coordinates	- layout (location=2) in vec2
 tangent				- layout (location=3) in vec3
 bi-tangent				- layout (location=4) in vec3

 */


class Model
{
public:
	/*  Model Data */
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;

	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const &path);

	// draws the model, and thus all its meshes
	void draw(GLuint shader);
	void attachTexture(GLuint textureID, string type = "texture_diffuse");

private:
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path);

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif

