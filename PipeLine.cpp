#include "PipeLine.h"

XMFLOAT3 CPipeLine::WorldViewTransform(XMFLOAT3& point) {
	// XMFLOAT4X4를 연산을 위해 XMMATRIX로 로드
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	// 행렬 곱 계산
	XMMATRIX total = world * view;

	// 받아온 정점과 전체 행렬을 곱하여 변환된 좌표 계산
	XMVECTOR transformed = XMVector3Transform(XMLoadFloat3(&point), total);
	// 변환된 좌표를 CVertex 형식으로 저장
	XMFLOAT3 result;
	XMStoreFloat3(&result, transformed);
	// 변환된 좌표를 반환
	return result;
}

XMFLOAT3 CPipeLine::ProjViewPortTransform(XMFLOAT3& point) {
	// XMFLOAT4X4를 연산을 위해 XMMATRIX로 로드
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewport = XMLoadFloat4x4(&mViewport);

	// 투영 변환
	XMVECTOR transformed = XMVector3Transform(XMLoadFloat3(&point), proj);

	// 원근 나누기
	float w = XMVectorGetW(transformed);
	if (fabs(w) > 0.000000001f) { // w가 0에 가까운 경우를 방지
		transformed = XMVectorDivide(transformed, XMVectorSplatW(transformed));
	}

	// 뷰포트 변환
	transformed = XMVector3Transform(transformed, viewport);

	// 변환된 좌표를 CVertex 형식으로 저장
	XMFLOAT3 result;
	XMStoreFloat3(&result, transformed);

	// 변환된 좌표를 반환
	return result;
}