#include "Camera.h"
#include <Windows.h>

Camera::Camera()
{
	gm_Posistion		= { 0,0,0 };
	gm_LookDirection	= { 0,0,1 };
	gm_UpDirection		= { 0,1,0 };
	gm_RightDirection	= { 1,0,0 };

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

	if (GetAsyncKeyState(VK_W))	
	{
		AddFloat3(gm_Posistion, gm_LookDirection);
	}
	if (GetAsyncKeyState(VK_S))
	{
		SubtractFloat3(gm_Posistion, gm_LookDirection);
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

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	UpdateQuaternion();
	DirectX::XMVECTOR position			= DirectX::XMLoadFloat3(&gm_Posistion);
	DirectX::XMVECTOR look_direction	= DirectX::XMLoadFloat3(&gm_LookDirection);
	DirectX::XMVECTOR up_direction		= DirectX::XMLoadFloat3(&gm_UpDirection);

	DirectX::XMFLOAT3X3 orientation_matrix;
	DirectX::XMStoreFloat3x3(&orientation_matrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&gm_CurrentQuaternion)));
	
	gm_RightDirection	= DirectX::XMFLOAT3(orientation_matrix._11, orientation_matrix._12, orientation_matrix._13);
	gm_UpDirection		= DirectX::XMFLOAT3(orientation_matrix._21, orientation_matrix._22, orientation_matrix._23);
	gm_LookDirection	= DirectX::XMFLOAT3(orientation_matrix._31, orientation_matrix._32, orientation_matrix._33);

	look_direction	= DirectX::XMLoadFloat3(&gm_LookDirection);
	up_direction	= DirectX::XMLoadFloat3(&gm_UpDirection);

	DirectX::XMVECTOR look_point = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(look_direction,MOVEMENT_SPEED));


	return DirectX::XMMatrixLookAtLH(position, look_point, up_direction);
}

void Camera::UpdateQuaternion()
{
	DirectX::XMVECTOR current_quaternion	= DirectX::XMLoadFloat4(&gm_CurrentQuaternion);
	DirectX::XMVECTOR to_be_quaternion		= DirectX::XMQuaternionRotationRollPitchYaw(gm_pitch, gm_yaw, gm_roll);

	DirectX::XMStoreFloat4(&gm_CurrentQuaternion, DirectX::XMQuaternionSlerp(current_quaternion, to_be_quaternion, ROTATION_SPEED));
}

void Camera::AddFloat3(DirectX::XMFLOAT3 &toAddTo, DirectX::XMFLOAT3 toAdd)
{
	DirectX::XMVECTOR to_add_to = DirectX::XMLoadFloat3(&toAddTo);
	DirectX::XMVECTOR to_add	= DirectX::XMVectorScale(DirectX::XMLoadFloat3(&toAdd),MOVEMENT_SPEED);

	DirectX::XMStoreFloat3(&toAddTo, DirectX::XMVectorAdd(to_add_to, to_add));
}
void Camera::SubtractFloat3(DirectX::XMFLOAT3 &toSubTo, DirectX::XMFLOAT3 toSub)
{
	DirectX::XMVECTOR to_sub_to = DirectX::XMLoadFloat3(&toSubTo);
	DirectX::XMVECTOR to_sub	= DirectX::XMVectorScale(DirectX::XMLoadFloat3(&toSub),MOVEMENT_SPEED);

	DirectX::XMStoreFloat3(&toSubTo, DirectX::XMVectorSubtract(to_sub_to, to_sub));
}
