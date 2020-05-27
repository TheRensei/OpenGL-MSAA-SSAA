#include "IslandSSAA.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "PrincipleAxes.h"
#include "UBOs.h"
#include <iostream>

using namespace std;

IslandSSAA::IslandSSAA(Camera *cam, int resolution[2])
{
	//Get pointer to the camera from the main scene
	islandCamera = cam;

	resolutionWidth = resolution[0];
	resolutionHeight = resolution[1];

	//
	// Setup FBO
	//

	glGenFramebuffers(1, &ssaaFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaaFBO);

	//
	// Setup textures that will be drawn into through the FBO
	//
	
	// Setup colour buffer texture.
	// Note:  The texture is stored as linear RGB values (GL_RGBA8). 
	glGenTextures(1, &fboColourTexture);
	glBindTexture(GL_TEXTURE_2D, fboColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolutionWidth, resolutionHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Setup depth texture

	glGenTextures(1, &fboDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, resolutionWidth, resolutionHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//
	// Attach textures to the FBO
	//

	// Attach the colour texture object to the framebuffer object's colour attachment point #0
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		fboColourTexture,
		0);

	// Attach the depth texture object to the framebuffer object's depth attachment point
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fboDepthTexture,
		0);

	//
	// Before proceeding make sure FBO can be used for rendering
	//

	GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {

		fboOkay = false;
		cout << "Could not successfully create framebuffer object to render texture!" << endl;

	}
	else {

		fboOkay = true;
		cout << "FBO successfully created" << endl;
	}

	// Unbind FBO for now! (Plug main framebuffer back in as rendering destination)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

IslandSSAA::~IslandSSAA()
{

}

Camera* IslandSSAA::getIslandSSAACamera()
{
	return islandCamera;
}

GLuint IslandSSAA::getIslandSSAATexture()
{
	return fboColourTexture;
}

void IslandSSAA::SetModels(Model inModels[4], GLuint VAO, Skybox *sky)
{
	models[0] = &inModels[0];
	models[1] = &inModels[1];
	models[2] = &inModels[2];
	models[3] = &inModels[3];
	pyramidVAO = VAO;
	skybox = sky;
}

void IslandSSAA::SetShader(GLuint multi, GLuint basic, GLuint camUBO) 
{
	multiLight = multi;
	basicShader = basic;
	cameraUBO = camUBO;
}

void IslandSSAA::render()
{
	if (!fboOkay)
		return; // Don't render anything if the FBO was not created successfully

	// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
	glBindFramebuffer(GL_FRAMEBUFFER, ssaaFBO);

	// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

	// Clear the screen (i.e. the texture)
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to specified texture size (see above)
	glViewport(0, 0, resolutionWidth, resolutionHeight);


	//Center the objects in the scene
	glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(0.02, 0.02, 0.02));
	model *= glm::translate(glm::mat4(1.0), glm::vec3(15.0f, 60.0f, -12.0f));
	model *= glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	
	// Get view-projection transform as a CGMatrix4
	glm::mat4 viewProjection = islandCamera->getProjectionMatrix() * islandCamera->getViewMatrix();

	glm::mat4 inverseTranspose = glm::transpose(glm::inverse(model));

	glUseProgram(multiLight);

	cameraTransformBuffer.model = model;
	cameraTransformBuffer.viewProjection = viewProjection;
	cameraTransformBuffer.invTranspose = inverseTranspose;
	cameraTransformBuffer.viewPosition = glm::vec4(islandCamera->getCameraPosition(), 1.0);

	//Send Camera transforms to the shader with UBO
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraTransformBuffer), &cameraTransformBuffer, GL_DYNAMIC_DRAW);

	//Render models here
	models[0]->draw(multiLight);
	models[1]->draw(multiLight);
	models[2]->draw(multiLight);
	models[3]->draw(multiLight);

	glUseProgram(0);

	glUseProgram(0);
	glUseProgram(basicShader);
	glm::mat4 modelShape = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 0.7f, 1.0f));
	modelShape *= glm::translate(glm::mat4(1.0), glm::vec3(0.1f, -0.8f, 0.2f));
	modelShape *= glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(modelShape));
	glUniformMatrix4fv(glGetUniformLocation(basicShader, "vpMatrix"), 1, GL_FALSE, glm::value_ptr(viewProjection));

	glBindVertexArray(pyramidVAO);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);

	//Render Skybox
	glm::mat4 skyModelMat = glm::scale(glm::mat4(1.0), glm::vec3(20.0, 20.0, 20.0));
	skyModelMat *= glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	skyModelMat *= glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	skybox->render(viewProjection * skyModelMat);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}