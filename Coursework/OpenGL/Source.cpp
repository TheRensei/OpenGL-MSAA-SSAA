#include "Includes.h"
#include "VertexData.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ 1000, 800, 0.1, 100.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data	//2.5f
Camera camera(camera_settings, glm::vec3(0.0f, 0.2f, 3.5f));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

bool			saveToFile = false;
ofstream		logCSV;

bool			showSSAAQuad = false;
IslandSSAA		*islandScene = nullptr;

//Super Sampling level
//Sets the resolution level and sample count level
int SSLevel = 2;

int SSresolution[2];
const char *windowTitle = "Coursework MSAA and SSAA";

glm::vec4 pointLightPositions[] = {
	glm::vec4(-1.0f, 0.5f, 0.0f, 1.0f), //Left
	glm::vec4(1.0f,  0.5f, 0.0f,1.0f) //Right
};

glm::vec4 pointLighsColours[] = {
	 glm::vec4(1.0f, 0.0, 0.0f, 1.0f), //Red
	 glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), //Blue
};

int main()
{
	SSresolution[0] = camera_settings.screenHeight * SSLevel;
	SSresolution[1] = camera_settings.screenWidth * SSLevel;

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Hint glfw how many samples we want to use
	//glfwWindowHint(GLFW_SAMPLES, 2);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, windowTitle, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Rendering settings
	glfwSwapInterval(0);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing
	//glEnable(GL_FRAMEBUFFER_SRGB);

	//Enable Multisampling
	//glEnable(GL_MULTISAMPLE);


	PrincipleAxes	*principleAxes = new PrincipleAxes();

	////	Shaders - Textures - Models	////
	GLuint multiLight;
	GLuint basicShader;

	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Multi_Light_Shader.vs"),
		string("Resources\\Shaders\\Multi_Light_Shader.fs"),
		&multiLight);

	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\basic_shader.vs"),
		string("Resources\\Shaders\\basic_shader.fs"),
		&basicShader);

	// UBO setup code for CameraMatrixBlockuniform block using std140 layout
	// Create a new UBO object to represent the camera data block
	GLuint cameraUBO;
	glGenBuffers(1, &cameraUBO);
	// Bind cameraUBOto binding point 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
	// Bind the CameraMatrixBlock in multilight binding point 0 (so the data we give the cameraUBO object will be picked up in the CameraMatrixBlock in the shader)
	GLuint multiLightCameraBlockIndex = glGetUniformBlockIndex(multiLight, "CameraMatrixBlock");
	glUniformBlockBinding(multiLight, multiLightCameraBlockIndex, 0);

	//Directional Light UBO
	GLuint dirLightUBO;
	glGenBuffers(1, &dirLightUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, dirLightUBO);
	GLuint multiLightDirLightBlockIndex = glGetUniformBlockIndex(multiLight, "DirLightBlock");
	glUniformBlockBinding(multiLight, multiLightDirLightBlockIndex, 2);

	GLuint pointLightUBO;
	glGenBuffers(1, &pointLightUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, pointLightUBO);
	GLuint multiLightPointLightBlockIndex = glGetUniformBlockIndex(multiLight, "PointLightBlock");
	glUniformBlockBinding(multiLight, multiLightPointLightBlockIndex, 3);

	//
	//UBOs Data Buffers
	//
	Transforms cameraTransformBuffer;
	DirLight dirLightBuffer;
	PointLight pointLightsBuffer[4];

	//
	//VAOS
	//

	GLuint	pyramidVertexBuffer;
	GLuint	pyramidColourBuffer;
	GLuint	pyramidIndexBuffer;

	GLuint	pyramidVAO;

	// Setup VAO
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// Setup VBO for vertex position data
	glGenBuffers(1, &pyramidVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr); // attribute 0 gets data from bound VBO (so assign vertex position buffer to attribute 0)

	// Setup VBO for vertex colour data
	glGenBuffers(1, &pyramidColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColours), pyramidColours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, nullptr); // attribute 1 gets colour data

	// Enable vertex position and colour attribute arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Setup VBO for face index array
	glGenBuffers(1, &pyramidIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidVertexIndices), pyramidVertexIndices, GL_STATIC_DRAW);

	// Unbind VAO (or bind another VAO for another object / effect)
	glBindVertexArray(0);


	//
	// Island scene
	//


	islandScene = new IslandSSAA(&camera, SSresolution);

	//
	// Texture quad which the IslandScene texture will be rendered on
	//

	TexturedQuad	*islandQuad = nullptr;
	islandQuad = new TexturedQuad(islandScene->getIslandSSAATexture(), SSresolution, SSLevel, true);

	//
	//SKybox - Models
	//

	Skybox skybox(
		string("Resources\\Models\\cubemap_space\\"),
		string("space"),
		string(".png")
	);

	Model models[4] = {
		Model("Resources\\Models\\treetop.obj"), //Tree Top
		Model("Resources\\Models\\treebase.obj"), // Tree Base
		Model("Resources\\Models\\vegetation.obj"), //Vegetation
		Model("Resources\\Models\\islandbase.obj") //IslandBase
	};

	//Send models to a FBO scene
	islandScene->SetModels(models, pyramidVAO, &skybox);
	islandScene->SetShader(multiLight, basicShader, cameraUBO);

	//
	//Send Directional Light Data to the shader
	//

	dirLightBuffer.direction = glm::vec4(0.2f, -1.0f, -0.5f, 1.0); //Light direction
	dirLightBuffer.ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0); //Light Ambient colour
	dirLightBuffer.diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0); //Light Diffuse colour
	dirLightBuffer.specular = glm::vec4(0.4f, 0.4f, 0.4f, 1.0); //Light specular colour

	glBindBuffer(GL_UNIFORM_BUFFER, dirLightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(dirLightBuffer), &dirLightBuffer, GL_DYNAMIC_DRAW);

	//
	//Send Point Lights Data to the shader
	//

	for (int i = 0; i < 2; i++)
	{
		pointLightsBuffer[i].position = pointLightPositions[i];

		pointLightsBuffer[i].constant = 1.0f;
		pointLightsBuffer[i].linear = 0.09f;
		pointLightsBuffer[i].quadratic = 0.032f;

		pointLightsBuffer[i].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
		pointLightsBuffer[i].diffuse = pointLighsColours[i];
		pointLightsBuffer[i].specular = glm::vec4(0.4f, 0.4f, 0.4f, 1.0);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, pointLightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(pointLightsBuffer), &pointLightsBuffer, GL_DYNAMIC_DRAW);

	// render loop									  
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		//
		// Render the FBO Island scene
		//

		if (showSSAAQuad)
		{
			islandScene->render();
		}


		//
		// Render the normal scene to the screen
		//

		// Clear the screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Reset the viewport
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		// Get view-projection transform as a CGMatrix4
		glm::mat4 viewProjection = projection * view;


		if (showSSAAQuad)
		{
			string title = string(windowTitle) + " : FBO Scene";
			glfwSetWindowTitle(window, title.c_str());

			if (islandQuad)
				islandQuad->render(viewProjection);
		}
		else
		{
			string title = string(windowTitle) + " : Normal Scene";
			glfwSetWindowTitle(window, title.c_str());

			glUseProgram(multiLight);
			//Center the objects in the scene
			glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(0.02, 0.02, 0.02));
			model *= glm::translate(glm::mat4(1.0), glm::vec3(15.0f, 60.0f, -12.0f));
			model *= glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));

			glm::mat4 inverseTranspose = glm::transpose(glm::inverse(model));

			cameraTransformBuffer.model = model;
			cameraTransformBuffer.viewProjection = viewProjection;
			cameraTransformBuffer.invTranspose = inverseTranspose;
			cameraTransformBuffer.viewPosition = glm::vec4(camera.getCameraPosition(), 1.0);

			//Send Camera transforms to the shader with UBO
			glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraTransformBuffer), &cameraTransformBuffer, GL_DYNAMIC_DRAW);


			models[0].draw(multiLight);
			models[1].draw(multiLight);
			models[2].draw(multiLight);
			models[3].draw(multiLight);


			glUseProgram(0);
			glUseProgram(basicShader);
			glm::mat4 modelShape = glm::scale(glm::mat4(1.0), glm::vec3(1.0f, 0.7f, 1.0f));
			modelShape *= glm::translate(glm::mat4(1.0), glm::vec3(0.1f, -0.8f, 0.2f));
			modelShape *= glm::rotate(glm::mat4(1.0), glm::radians(180.0f ),glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(modelShape));
			glUniformMatrix4fv(glGetUniformLocation(basicShader, "vpMatrix"), 1, GL_FALSE, glm::value_ptr(viewProjection));

			glBindVertexArray(pyramidVAO);
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, nullptr);
			glBindVertexArray(0);

			glUseProgram(0);

			//Render Skybox
			glm::mat4 skyModelMat = glm::scale(glm::mat4(1.0), glm::vec3(20.0, 20.0, 20.0));
			skyModelMat *= glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
			skyModelMat *= glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			skybox.render(viewProjection * skyModelMat);
		}

		//Log file with PFS and SDF
		if (saveToFile)
		{
			string title = string(windowTitle) + " : Saving to file...";
			glfwSetWindowTitle(window, title.c_str());

			if (!logCSV.is_open())
			{
				logCSV.open("log.csv");
			}
			logCSV << std::to_string(timer.averageFPS()) + ',' + std::to_string(timer.currentSPF()) + '\n';
		}
		else
		{
			if (logCSV.is_open())
			{
				logCSV.close();
			}
		}


		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		showSSAAQuad = !showSSAAQuad;
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		saveToFile = !saveToFile;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xoffset, yoffset);
	}

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}