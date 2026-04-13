#include "GameObject.h"

void CGameObject::SetWorldMatrix() {
	// 객체 회전을 쿼터니언으로 변환
	XMVECTOR quatRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&Rotation));
	// 회전 행렬 생성
	XMMATRIX mRotation = XMMatrixRotationQuaternion(quatRotation);
	// 이동 행렬 생성
	XMMATRIX mTranslation = XMMatrixTranslation(Position.x, Position.y, Position.z);
	// 월드 행렬 계산
	XMMATRIX mWorld = mRotation * mTranslation;
	// 월드 행렬을 XMFLOAT4X4로 변환하여 저장
	XMStoreFloat4x4(&WorldMatrix, mWorld);
}

void CGameObject::Draw(HDC hDC, CPipeLine& pipeline) {
	if (mesh) {
		mesh->Draw(hDC, pipeline, MeshColor);
	}
}

void CGameObject::Animate(float time) {
	// 회전 애니메이션
	Rotation.y += 0.01f; // Y축을 기준으로 회전
	if (Rotation.y > XM_2PI) {
		Rotation.y -= XM_2PI; // 360도 이상 회전하면 초기화
	}
}