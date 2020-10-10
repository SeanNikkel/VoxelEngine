#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Handles camera data and matrix generation
class Camera
{
public:
	Camera(glm::vec3 position, float fov = 75.0f);

	// Camera getters
	glm::vec3 GetPosition() const;
	float GetFov() const;
	float GetPitch() const;
	float GetYaw() const;
	float GetNearPlane() const;
	float GetFarPlane() const;

	// Matrix getters
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetPerspectiveMatrix(glm::vec2 screenSize) const;

	// Vector getters
	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetForwardAligned() const;
	glm::vec3 GetUpAligned() const;

	// Movement setters
	void MoveForward(float amount);
	void MoveRight(float amount);
	void MoveUp(float amount);

	// Camera setters
	void SetPosition(glm::vec3 pos);
	void SetFov(float fov);
	void SetPitch(float pitch);
	void SetYaw(float yaw);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);

private:
	glm::vec3 pos_;
	float fov_;
	float pitch_ = 0;
	float yaw_ = 0;
	float nearPlane_ = 0.1f;
	float farPlane_ = 2000.0f;
};