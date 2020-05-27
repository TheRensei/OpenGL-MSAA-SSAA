#include "Camera.h"

Camera::Camera(Camera_settings camera_settings)
{
	this->ScreenWidth = camera_settings.screenWidth;
	this->ScreenHeight = camera_settings.screenHeight;
	this->NearPlane = camera_settings.nearPlane;
	this->FarPlane = camera_settings.farPlane;

	this->MovementSpeed = SPEED;
	this->MouseSensitivity = SENSITIVITY;
	this->Zoom = ZOOM;
	this->Yaw = YAW;
	this->Pitch = PITCH;

	this->Position = glm::vec3(0.0f, 0.5f, 5.0f);
	this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->Target = glm::vec3(0.0f, 0.0f, -1.0f);
	updateCameraVectors();
}

// Constructor with position vector
Camera::Camera(Camera_settings camera_settings, glm::vec3 position)
{
	this->ScreenWidth = camera_settings.screenWidth;
	this->ScreenHeight = camera_settings.screenHeight;
	this->NearPlane = camera_settings.nearPlane;
	this->FarPlane = camera_settings.farPlane;

	this->MovementSpeed = SPEED;
	this->MouseSensitivity = SENSITIVITY;
	this->Zoom = ZOOM;
	this->Yaw = YAW;
	this->Pitch = PITCH;

	this->Position = position;
	this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->Target = glm::vec3(0.0f, 0.0f, -1.0f);
	updateCameraVectors();
}

// Constructor with vectors
Camera::Camera(Camera_settings camera_settings, glm::vec3 position, glm::vec3 target, glm::vec3 up, double yaw, double pitch )
{
	this->ScreenWidth = camera_settings.screenWidth;
	this->ScreenHeight = camera_settings.screenHeight;
	this->NearPlane = camera_settings.nearPlane;
	this->FarPlane = camera_settings.farPlane;

	this->MovementSpeed = SPEED;
	this->MouseSensitivity = SENSITIVITY;
	this->Zoom = ZOOM;

	this->Position = position;
	this->Target = target;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	updateCameraVectors();
}

// Constructor with scalar values
Camera::Camera(Camera_settings camera_settings, double posX, double posY, double posZ, double upX, double upY, double upZ, double tarX, double tarY, double tarZ, double yaw, double pitch)
{
	this->ScreenWidth = camera_settings.screenWidth;
	this->ScreenHeight = camera_settings.screenHeight;
	this->NearPlane = camera_settings.nearPlane;
	this->FarPlane = camera_settings.farPlane;

	this->Target = glm::vec3(tarX, tarY, -tarZ);
	this->MovementSpeed = SPEED;
	this->MouseSensitivity = SENSITIVITY;
	this->Zoom = ZOOM;
	this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(this->Position, this->Position + this->Target, this->Up);
}

// Returns the projection matrix calculated using screen size, zoom, near and far planes
glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(this->Zoom), static_cast<double>(this->ScreenWidth) / static_cast<double>(this->ScreenHeight), this->NearPlane, this->FarPlane);
}

glm::vec3 Camera::getCameraPosition()
{
	return this->Position;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::processKeyboard(Camera_Movement direction, double deltaTime)
{
	float velocity = static_cast<float>(this->MovementSpeed * deltaTime);
	if (direction == FORWARD)
		this->Position += this->Target * velocity;
	if (direction == BACKWARD)
		this->Position -= this->Target * velocity;
	if (direction == LEFT)
		this->Position -= this->Right * velocity;
	if (direction == RIGHT)
		this->Position += this->Right * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch)
{
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw += xoffset;
	this->Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(double yoffset)
{
	if (this->Zoom >= 1.0f && this->Zoom <= 100.0f)
		this->Zoom -= yoffset;
	if (this->Zoom <= 1.0f)
		this->Zoom = 1.0f;
	if (this->Zoom >= 100.0f)
		this->Zoom = 100.0f;
}

// Updates the screen width and height data to return correct view matrix if screen size has changed
void Camera::updateScreenSize(double width, double height)
{
	this->ScreenWidth = static_cast<unsigned int>(width);
	this->ScreenHeight = static_cast<unsigned int>(height);
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 target;
	target.x = static_cast<float>(cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch)));
	target.y = static_cast<float>(sin(glm::radians(this->Pitch)));
	target.z = static_cast<float>(sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch)));
	this->Target = glm::normalize(target);
	// Also re-calculate the Right and Up vector
	this->Right = glm::normalize(glm::cross(this->Target, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->Up = glm::normalize(glm::cross(this->Right, this->Target));
}