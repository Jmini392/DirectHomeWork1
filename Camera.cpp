#include "PCH.h"
#include "Camera.h"

CCamera::CCamera() {
	XMStoreFloat4x4(&cameraMatrix, XMMatrixIdentity());
}

void CCamera::SetCamera() {
	// 카메라의 위치와 방향을 설정한다.
	SetPosition(0.f, 0.f, 0.f);
	SetRotattion(0.f, 0.f, 0.f);
}

void CCamera::SetPosition(float x, float y, float z) {
	// 카메라의 위치를 설정한다.
	cameraMatrix._41 = x;
	cameraMatrix._42 = y;
	cameraMatrix._43 = z;
}

void CCamera::SetRotattion(float x, float y, float z) {
	// 카메라의 회전 각도를 설정한다.
	// 기존의 위치(Position) 값 백업
	float tx = cameraMatrix._41;
	float ty = cameraMatrix._42;
	float tz = cameraMatrix._43;
	float tw = cameraMatrix._44;

	// X, Y, Z축 순서로 회전된 회전 행렬 생성
	XMMATRIX mRot = XMMatrixRotationRollPitchYaw(x, y, z);

	// 회전 행렬을 적용하고 기존 위치 정보를 다시 복원
	XMStoreFloat4x4(&cameraMatrix, mRot);
	cameraMatrix._41 = tx;
	cameraMatrix._42 = ty;
	cameraMatrix._43 = tz;
	cameraMatrix._44 = tw;
}