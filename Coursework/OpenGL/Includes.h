#ifndef INCLUDES_H
#define INCLUDES_H
//Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>


//Local files
#include "Timer.h"
#include "Model.h"
#include "Circle.h"
#include "Camera.h"
#include "Sphere.h"
#include "Skybox.h"
#include "TexturedQuad.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "PrincipleAxes.h"
#include "IslandSSAA.h"
#include "UBOs.h"

//------------------------------

//------------------------------

//namespaces
using std::string;

#endif