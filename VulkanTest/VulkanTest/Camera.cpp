#include "Camera.h"
#include <Windows.h>

Camera::Camera()
{
	gm_Posistion = { 0,0,0 };
	gm_LookDirection = { 0,0,1 };
	gm_UpDirection = { 0,1,0 };
	gm_RightDirection = { 1,0,0 };

	DirectX::XMStoreFloat4(&gm_CurrentQuaternion, DirectX::XMQuaternionIdentity());

	gm_roll = 0.0f;
	gm_yaw = 0.0f;
	gm_pitch = 0.0f;
}

void Camera::HandleInput()
{
	if (GetAsyncKeyState(VK_DOWN))
	{
		gm_pitch += ROTATION_SPEED;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		gm_pitch -= ROTATION_SPEED;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		gm_yaw -= ROTATION_SPEED;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		gm_yaw += ROTATION_SPEED;
	}
	if (GetAsyncKeyState(VK_E))
	{
		gm_roll += ROTATION_SPEED;
	}
	if (GetAsyncKeyState(VK_Q))
	{
		gm_roll -= ROTATION_SPEED;
	}
	if (GetAsyncKeyState(VK_W))
	{
		SubtractFloat3(gm_Posistion, gm_LookDirection);
	}
	if (GetAsyncKeyState(VK_S))
	{
		AddFloat3(gm_Posistion, gm_LookDirection);
	}
	if (GetAsyncKeyState(VK_D))
	{
		AddFloat3(gm_Posistion, gm_RightDirection);
	}
	if (GetAsyncKeyState(VK_A))
	{
		SubtractFloat3(gm_Posistion, gm_RightDirection);
	}
	if (GetAsyncKeyState(VK_R))
	{
		ResetCamera();
	}

	if (GetAsyncKeyState(VK_SPACE))
	{
		AddFloat3(gm_Posistion, gm_UpDirection);
	}
	if (GetAsyncKeyState(VK_CONTROL))
	{
		SubtractFloat3(gm_Posistion, gm_UpDirection);
	}

}

void Camera::ResetCamera()
{
	gm_Posistion = { 0,0,0 };
	gm_LookDirection = { 0,0,1 };
	gm_UpDirection = { 0,1,0 };
	gm_RightDirection = { 1,0,0 };

	DirectX::XMStoreFloat4(&gm_CurrentQuaternion, DirectX::XMQuaternionIdentity());

	gm_roll = 0.0f;
	gm_yaw = 0.0f;
	gm_pitch = 0.0f;
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::quat key_quat = glm::quat(glm::vec3(gm_pitch, gm_yaw, gm_roll));

	gm_pitch = gm_yaw = gm_roll = 0;

	camera_quat = key_quat * camera_quat;
	camera_quat = glm::normalize(camera_quat);
	glm::mat4 rotate = glm::mat4_cast(camera_quat);

	glm::vec3 a = rotate[0];
	glm::vec3 b = rotate[1];
	glm::vec3 c = rotate[2];

	gm_RightDirection	= DirectX::XMFLOAT3(a.x, b.x, c.x);
	gm_UpDirection		= DirectX::XMFLOAT3(a.y, b.y, c.y);
	gm_LookDirection	= DirectX::XMFLOAT3(a.z, b.z, c.z);

	glm::mat4 translate = glm::mat4(1.0f);
	translate = glm::translate(translate, glm::vec3(-gm_Posistion.x, -gm_Posistion.y, -gm_Posistion.z));

	return rotate * translate;
}

void Camera::UpdateQuaternion()
{
	DirectX::XMVECTOR current_quaternion = DirectX::XMLoadFloat4(&gm_CurrentQuaternion);
	DirectX::XMVECTOR to_be_quaternion = DirectX::XMQuaternionRotationRollPitchYaw(gm_pitch, gm_yaw, gm_roll);

	DirectX::XMStoreFloat4(&gm_CurrentQuaternion, DirectX::XMQuaternionSlerp(current_quaternion, to_be_quaternion, ROTATION_SPEED));
}

DirectX::XMFLOAT3 Camera::GetPosition() {
	return gm_Posistion;
}
DirectX::XMFLOAT3 Camera::GetLookAt() {
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&gm_Posistion);
	DirectX::XMVECTOR look_direction = DirectX::XMLoadFloat3(&gm_LookDirection);

	DirectX::XMVECTOR look_point = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(look_direction, MOVEMENT_SPEED));
	DirectX::XMFLOAT3 r;
	DirectX::XMStoreFloat3(&r, look_point);
	return r;
}
DirectX::XMFLOAT3 Camera::GetUpDir() {
	return gm_UpDirection;
}

void Camera::AddFloat3(DirectX::XMFLOAT3 &toAddTo, DirectX::XMFLOAT3 toAdd)
{
	DirectX::XMVECTOR to_add_to = DirectX::XMLoadFloat3(&toAddTo);
	DirectX::XMVECTOR to_add = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&toAdd), MOVEMENT_SPEED);

	DirectX::XMStoreFloat3(&toAddTo, DirectX::XMVectorAdd(to_add_to, to_add));
}
void Camera::SubtractFloat3(DirectX::XMFLOAT3 &toSubTo, DirectX::XMFLOAT3 toSub)
{
	DirectX::XMVECTOR to_sub_to = DirectX::XMLoadFloat3(&toSubTo);
	DirectX::XMVECTOR to_sub = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&toSub), MOVEMENT_SPEED);

	DirectX::XMStoreFloat3(&toSubTo, DirectX::XMVectorSubtract(to_sub_to, to_sub));
}

void Camera::test() {
	glm::quat key_quat = glm::quat(glm::vec3(gm_pitch, gm_yaw, gm_roll));

	gm_pitch = gm_yaw = gm_roll = 0;

	camera_quat = key_quat * camera_quat;
	camera_quat = glm::normalize(camera_quat);
	glm::mat4 rotate = glm::mat4_cast(camera_quat);
}
