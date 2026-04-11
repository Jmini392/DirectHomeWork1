#include "PipeLine.h"

XMFLOAT3 CPipeLine::MatrixTransform(XMFLOAT3& point) {
	// XMFLOAT4X4를 연산을 위해 XMMATRIX로 로드
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewport = XMLoadFloat4x4(&mViewport);

	// 행렬 곱 계산
	XMMATRIX total = world * view * proj * viewport;
	
	// 받아온 정점과 전체 행렬을 곱하여 변환된 좌표 계산
	XMVECTOR transformed = XMVector3Transform(XMLoadFloat3(&point), total);

	// 원근 나누기
	float w = XMVectorGetW(transformed);
	transformed = XMVectorDivide(transformed, XMVectorSplatW(transformed));

	// 변환된 좌표를 CVertex 형식으로 저장
	XMFLOAT3 result;
	XMStoreFloat3(&result, transformed);

	// 변환된 좌표를 반환
	return result;
}

bool CPipeLine::CameraDot(XMFLOAT3& vector) {
	// 카메라의 시선 방향과 벡터의 내적 계산
	XMVECTOR camLook = XMLoadFloat3(&vector);
	XMVECTOR camDir = XMLoadFloat3(&m_CameraLookVector); // 카메라의 시선 방향 벡터 (LOOK)

	float dotProduct = XMVectorGetX(XMVector3Dot(camLook, camDir));

	// 내적이 양수이면 앞면, 음수이면 뒷면
	return dotProduct > 0;
}