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
	// 기본적으로 아무 동작도 하지 않음
}

void CRotate::Animate(float time) {
	// 회전 애니메이션
	float y;
	y = GetRotation().y;
	y += 1.f * time; // Y축을 기준으로 회전
	if (y > XM_2PI) y -= XM_2PI; // 360도 이상 회전하면 초기화
	SetRotation(GetRotation().x, y, GetRotation().z);
}

void CBullet::Animate(float time) {
	XMFLOAT3 pos = GetPosition();
	
	pos = Vector3::Add(pos, Vector3::ScalarProduct(Direction, Speed * time)); // 방향 벡터를 위치에 더해서 이동
	
	SetPosition(pos.x, pos.y, pos.z);

	// 총알이 생기고 일정거리 멀어지면 소멸 처리
	XMFLOAT3 offset = Vector3::Subtract(pos, StartPosition);
	if (Vector3::Length(offset) > 10.f) isdead = true;
}