#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

const float ROTATION_SPEED = 0.005f;
const float MOVEMENT_SPEED = 0.01f;
class Camera
{
private:
	DirectX::XMFLOAT3 gm_Posistion;
	DirectX::XMFLOAT3 gm_LookDirection;
	DirectX::XMFLOAT3 gm_UpDirection;
	DirectX::XMFLOAT3 gm_RightDirection;

	DirectX::XMFLOAT4 gm_CurrentQuaternion;

	float gm_roll;
	float gm_yaw;
	float gm_pitch;

	#define VK_W 0x57
	#define VK_S 0x53
	#define VK_D 0x44
	#define VK_A 0x41
	#define VK_R 0x52


	void UpdateQuaternion();
	void AddFloat3(DirectX::XMFLOAT3 &toAddTo, DirectX::XMFLOAT3 toAdd); //add to helper/constants class
	void SubtractFloat3(DirectX::XMFLOAT3 &toSubTo, DirectX::XMFLOAT3 toSub); //add to helper/constants class

public:
	Camera();

	void HandleInput();

	DirectX::XMMATRIX GetViewMatrix();

	void ResetCamera();

};



#endif