#include "Camera.h"
#include "Math.h"

Camera::Camera(glm::vec3 position, float fov)
{
	SetPosition(position);
	SetFov(fov);
}

glm::vec3 Camera::GetPosition() const
{
	return pos_;
}

float Camera::GetFov() const
{
	return fov_;
}

float Camera::GetPitch() const
{
	return pitch_;
}

float Camera::GetYaw() const
{
	return yaw_;
}

float Camera::GetNearPlane() const
{
	return nearPlane_;
}

float Camera::GetFarPlane() const
{
	return farPlane_;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(pos_, pos_ + GetForward(), GetUp());
}

glm::mat4 Camera::GetPerspectiveMatrix(glm::vec2 screenSize) const
{
	if (screenSize.x <= 0 || screenSize.y <= 0)
		return glm::mat4();

	return glm::perspective(glm::radians(fov_), screenSize.x / screenSize.y, nearPlane_, farPlane_);
}

glm::vec3 Camera::GetForward() const
{
	// Spherical coordinates
	return glm::normalize(glm::vec3(
		cos(pitch_) * sin(yaw_),
		sin(pitch_),
		cos(pitch_) * cos(yaw_)
	));
}

glm::vec3 Camera::GetRight() const
{
	float angle = yaw_ - glm::half_pi<float>();

	return glm::vec3(
		sin(angle),
		0,
		cos(angle)
	);
}

glm::vec3 Camera::GetUp() const
{
	return glm::cross(GetRight(), GetForward());
}

glm::vec3 Camera::GetForwardAligned() const
{
	return glm::vec3(
		sin(yaw_),
		0,
		cos(yaw_)
	);
}

glm::vec3 Camera::GetUpAligned() const
{
	return glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::MoveForward(float amount)
{
	pos_ += GetForward() * amount;
}

void Camera::MoveRight(float amount)
{
	pos_ += GetRight() * amount;
}

void Camera::MoveUp(float amount)
{
	pos_ += GetUp() * amount;
}

void Camera::SetPosition(glm::vec3 pos)
{
	pos_ = pos;
}

void Camera::SetFov(float fov)
{
	fov_ = glm::clamp(fov, 0.0f, 180.0f);
}

void Camera::SetPitch(float pitch)
{
	pitch_ = glm::clamp(pitch, -glm::half_pi<float>(), glm::half_pi<float>());
}

void Camera::SetYaw(float yaw)
{
	yaw_ = Math::PositiveMod(yaw, glm::two_pi<float>());
}

void Camera::SetNearPlane(float nearPlane)
{
	nearPlane_ = glm::clamp(nearPlane, 0.0f, INFINITY);
}

void Camera::SetFarPlane(float farPlane)
{
	 farPlane_ = glm::clamp(farPlane, 0.0f, INFINITY);
}
