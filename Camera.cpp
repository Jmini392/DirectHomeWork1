#include "PCH.h"
#include "Camera.h"

void CViewport::SetProjMatrix() {
	// 원근 투영 변환 행렬을 생성한다.
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(Fov), Aspect, Near, Far);
	ProjectionMatrix = Matrix4x4::Identity();
	XMStoreFloat4x4(&ProjectionMatrix, mProj);
}

void CViewport::SetViewportMatrix() {
	// 1. ViewportMatrix를 단위 행렬로 초기화
	ViewportMatrix = Matrix4x4::Identity();

	// 2. NDC(-1 ~ 1) 좌표를 화면 픽셀 좌표로 변환하는 공식 적용
	// X 좌표: [-1, 1] -> [ViewportX, ViewportX + ViewportWidth]
	ViewportMatrix._11 = ViewportWidth / 2.0f;
	ViewportMatrix._41 = ViewportX + (ViewportWidth / 2.0f);

	// Y 좌표: [1, -1] -> [ViewportY, ViewportY + ViewportHeight] (GDI Y축 반전 반영)
	ViewportMatrix._22 = -ViewportHeight / 2.0f;
	ViewportMatrix._42 = ViewportY + (ViewportHeight / 2.0f);

	// Z 좌표: [0, 1] 유지
	ViewportMatrix._33 = 1.0f;
}

void CCamera::SetCamera() { }

void CCamera::SetViewMatrix() {	
	// Look을 AT과 EYE의 차이로 설정
	LOOK = Vector3::Subtract(AT, EYE);
	
	// Right를 Look과 UP의 외적으로 설정
	RIGHT = Vector3::CrossProduct(LOOK, UP);
	
	// UP을 Right와 Look의 외적으로 설정
	UP = Vector3::CrossProduct(RIGHT, LOOK);
	
	// Look과 Right, UP 벡터를 정규화
	LOOK = Vector3::Normalize(LOOK);
	RIGHT = Vector3::Normalize(RIGHT);
	UP = Vector3::Normalize(UP);
	
	// LookAtLH 함수를 사용하여 ViewMatrix 설정
	ViewMatrix = Matrix4x4::LookAtLH(EYE, AT, UP);
}