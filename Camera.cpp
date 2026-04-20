#include "Camera.h"

CCamera::CCamera() {
	m_Viewport = std::make_unique<CViewport>();
}

void CCamera::Move(float x, float y, float z) {
	EYE.x += x; EYE.y += y; EYE.z += z;
	SetViewMatrix();
}

void CCamera::Rotate(float x, float y, float z) {
	Rotation.x += x; Rotation.y += y; Rotation.z += z;
	SetViewMatrix();
}

void CCamera::SetViewMatrix() {	
	// 카메라의 회전(Rotation) 각도를 라디안으로 변환
	float pitch = XMConvertToRadians(Rotation.x);
	float yaw = XMConvertToRadians(Rotation.y);

	// 구면 좌표계 공식을 이용하여 회전된 전방향(Look) 벡터 계산
	float fLookX = sinf(yaw) * cosf(pitch);
	float fLookY = sinf(pitch);
	float fLookZ = cosf(yaw) * cosf(pitch);

	// 산출된 전방향을 현재 위치(EYE)에 더해서 바라보는 목표물(AT) 갱신
	AT.x = EYE.x + fLookX;
	AT.y = EYE.y + fLookY;
	AT.z = EYE.z + fLookZ;

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

void CCamera::SetProjMatrix() {
	// 원근 투영 변환 행렬을 생성한다.
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Viewport->Fov), m_Viewport->Aspect, m_Viewport->Near, m_Viewport->Far);

	// 카메라 클래스의 ProjectionMatrix 멤버 변수에 원근 투영 변환 행렬을 저장한다
	ProjectionMatrix = Matrix4x4::Identity();
	XMStoreFloat4x4(&ProjectionMatrix, mProj);
}

void CCamera::SetViewportMatrix() {
	// 단위 행렬로 초기화
	ViewportMatrix = Matrix4x4::Identity();

	// NDC(-1 ~ 1) 좌표를 화면 픽셀 좌표로 변환하는 공식 적용
	// X 좌표: [-1, 1] -> [ViewportX, ViewportX + ViewportWidth]
	ViewportMatrix._11 = m_Viewport->ViewportWidth / 2.0f;
	ViewportMatrix._41 = m_Viewport->ViewportX + (m_Viewport->ViewportWidth / 2.0f);

	// Y 좌표: [1, -1] -> [ViewportY, ViewportY + ViewportHeight] (GDI Y축 반전 반영)
	ViewportMatrix._22 = -m_Viewport->ViewportHeight / 2.0f;
	ViewportMatrix._42 = m_Viewport->ViewportY + (m_Viewport->ViewportHeight / 2.0f);

	// Z 좌표: [0, 1] 유지
	ViewportMatrix._33 = 1.0f;
}