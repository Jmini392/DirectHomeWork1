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

CWall::CWall() {
	SetType(ObjectType::WALL);
	// 메시 크기 객체 고정
	std::shared_ptr<CMesh> pWallMesh = std::make_shared<CCubeMesh>(2.0f, 100.f, 140.f);
	// 벽 객체 초기화
	SetMesh(pWallMesh);
}

CFloor::CFloor() {
	SetType(ObjectType::FLOOR);
	// 메시 크기 객체 고정
	std::shared_ptr<CMesh> pFloorMesh = std::make_shared<CCubeMesh>(20.f, 1.f, 20.f);
	// 바닥 객체 초기화
	SetMesh(pFloorMesh);
}

CItem::CItem() {
	SetType(ObjectType::ITEM);
	randomValue = RANDOM;
	// 메시 크기 객체 고정
	std::shared_ptr<CMesh> pItemMesh = std::make_shared<CCubeMesh>(1.5f, 1.5f, 1.5f);
	// 아이템 객체 초기화
	SetMesh(pItemMesh);
	if (randomValue == 0) SetColor(RGB(255, 0, 255));
	else SetColor(RGB(0, 255, 255));
	// 위치는 맵안에서 랜덤하게 생성
}

void CItem::Animate(float time) {
	// 회전 애니메이션
	float y;
	y = GetRotation().y;
	y += 1.f * time; // Y축을 기준으로 회전
	if (y > XM_2PI) y -= XM_2PI; // 360도 이상 회전하면 초기화
	SetRotation(GetRotation().x, y, GetRotation().z);
}

void CItem::OnCollision(std::shared_ptr<CGameObject> pOther) {
	if (pOther->GetType() == ObjectType::PLAYER) {
		isdead = true;
	}
}

void CBullet::Animate(float time) {
	XMFLOAT3 pos = GetPosition();
	
	pos = Vector3::Add(pos, Vector3::ScalarProduct(Direction, Speed * time)); // 방향 벡터를 위치에 더해서 이동
	
	CGameObject::SetPosition(pos.x, pos.y, pos.z);

	// 총알이 생기고 일정거리 멀어지면 소멸 처리
	XMFLOAT3 offset = Vector3::Subtract(pos, StartPosition);
	// 사거리 20.f 이상이면 총알 소멸
	if (Vector3::Length(offset) > 20.f) isdead = true;
}

void CBullet::OnCollision(std::shared_ptr<CGameObject> pOther) {
	ObjectType otherType = pOther->GetType();
	if (otherType == ObjectType::ENEMY || otherType == ObjectType::WALL) {
		isdead = true;
	}
}