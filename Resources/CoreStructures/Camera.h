#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several enum possible options for camera movement to easier understand the direction
enum Camera_Movement 
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Struct to store and pass camera settings
struct Camera_settings
{
	unsigned int screenWidth;
	unsigned int screenHeight;
	double nearPlane;
	double farPlane;
};

// Default camera values
const double YAW = -90.0f;
const double PITCH = 0.0f;
const double SPEED = 2.5f;
const double SENSITIVITY = 0.1f;
const double ZOOM = 45.0f;

// Camera class
class Camera
{
private:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Target;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Euler Angles
	double Yaw;
	double Pitch;

	// Camera options
	double MovementSpeed;
	double MouseSensitivity;
	double Zoom;
	double NearPlane;
	double FarPlane;

	// Screen options
	unsigned int ScreenWidth;
	unsigned int ScreenHeight;

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();

public:

	// Default Constructor
	Camera(Camera_settings camera_settings);

	// Constructor with vectors
	Camera(Camera_settings camera_settings, glm::vec3 position);

	// Constructor with vectors
	Camera(Camera_settings camera_settings, glm::vec3 position, glm::vec3 target, glm::vec3 up, double yaw, double pitch);

	// Constructor with scalar values
	Camera(Camera_settings camera_settings, double posX, double posY, double posZ, double upX, double upY, double upZ, double tarX, double tarY, double tarZ, double yaw, double pitch);

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 getViewMatrix();

	// Returns the projection matrix calculated using screen size, zoom, near and far planes
	glm::mat4 getProjectionMatrix();

	// Returns the camera position
	glm::vec3 getCameraPosition();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void processKeyboard(Camera_Movement direction, double deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void processMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch = true);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void processMouseScroll(double yoffset);

	// Updates the screen width and height data to return correct view matrix if screen size has changed
	void updateScreenSize(double width, double height);
};

#endif

