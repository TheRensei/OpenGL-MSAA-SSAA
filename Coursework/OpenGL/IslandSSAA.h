#ifndef ISLAND_SSAA_H
#define ISLAND_SSAA_H

#include "Model.h"
#include "Camera.h"
#include "UBOs.h"
#include "Skybox.h"

class IslandSSAA {

private:
	Model *models[4];
	GLuint pyramidVAO;

	Skybox *skybox;

	Camera *islandCamera;

	GLuint multiLight;
	GLuint basicShader;

	GLuint cameraUBO;
	Transforms cameraTransformBuffer;

	GLuint ssaaFBO;
	GLuint fboColourTexture;
	GLuint fboDepthTexture;
	
	bool fboOkay;

	int resolutionWidth;
	int resolutionHeight;

public:
	IslandSSAA(Camera*, int[2]);
	~IslandSSAA();

	Camera* getIslandSSAACamera();
	GLuint getIslandSSAATexture();
	
	void SetModels(Model[4], GLuint, Skybox*);
	void SetShader(GLuint, GLuint, GLuint);

	void render();

};

#endif
