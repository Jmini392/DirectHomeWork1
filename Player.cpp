#include "Player.h"
#include "Scene.h"

CPlayer::CPlayer() {
	SetType(ObjectType::PLAYER);
	// 메시 크기 객체 고정
	std::shared_ptr<CMesh> pEnemyMesh = std::make_shared<CCubeMesh>(4.f, 4.f, 4.f);
	// 적 객체 초기화
	SetMesh(pEnemyMesh);
}

void CPlayer::SetPosition(float x, float y, float z) {
	CGameObject::SetPosition(x, y, z);
	if (m_pCamera) m_pCamera->SetPosition(x, y, z);
}

void CPlayer::SetRotation(float x, float y, float z) {
	CGameObject::SetRotation(x, y, z);
	if (m_pCamera) m_pCamera->SetRotation(x, y, z);
	// 회전 후 방향 벡터 업데이트
	XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	direction = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);
}

void CPlayer::Move(int dir) {
	if (dir == 0) return;
	
	// 이동 전 현재 위치 저장
	m_PrevPosition = GetPosition();
	
	XMFLOAT3 moveVec = direction;
	int moveSign = dir;
	if (dir == 2 || dir == -2) {
		moveVec = XMFLOAT3(-direction.z, direction.y, direction.x);
		moveSign = (dir == -2) ? 1 : -1;
	}
	
	CGameObject::Move(moveVec.x * moveSign * MoveSpeed, moveVec.y * moveSign * MoveSpeed, moveVec.z * moveSign * MoveSpeed);
	if (m_pCamera) m_pCamera->Move(moveVec.x * moveSign * MoveSpeed, moveVec.y * moveSign * MoveSpeed, moveVec.z * moveSign * MoveSpeed);
}

void CPlayer::Rotate(float x, float y, float z) {
	CGameObject::Rotate(x, y, z);
	if (m_pCamera) m_pCamera->Rotate(x, y, z);
	// 회전 후 방향 벡터 업데이트
	XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	direction = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);
}

std::shared_ptr<CGameObject> CPlayer::Fire() {
	// 총알 객체를 동적으로 생성
	if (bulletCount <= 0) return nullptr; // 총알이 없으면 발사하지 않음
	bulletCount--;
	std::shared_ptr<CMesh> pSphereMesh = std::make_shared<CObjMesh>("Sphere.obj");
	
	std::shared_ptr<CBullet> pBullet = std::make_shared<CBullet>();
	pBullet->SetMesh(pSphereMesh);
	pBullet->SetColor(RGB(255, 255, 50));

	// 발사 방향(Forward)을 먼저 계산합니다.
	XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	XMFLOAT3 forward = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);
	
	XMVECTOR vForward = XMLoadFloat3(&forward);
	vForward = XMVector3Normalize(vForward);
	XMStoreFloat3(&forward, vForward);

	pBullet->SetDirection(forward);
	pBullet->SetSpeed(ShootSpeed); 

	// 플레이어 위치를 기준으로 총알의 생성 위치를 정합니다. 
	XMFLOAT3 pos = GetPosition();
	float spawnOffset = 2.0f; // 플레이어의 중심에서 앞쪽으로 떨어뜨릴 거리 지정
	
	// 현재 위치에 전방 벡터 * 일정 거리
	pBullet->SetPosition(pos.x + (forward.x * spawnOffset),pos.y, pos.z + (forward.z * spawnOffset));

	// 총알 객체 반환
	return pBullet;
}

void CPlayer::OnCollision(std::shared_ptr<CGameObject> pOther) {
	ObjectType otherType = pOther->GetType();
	if (otherType == ObjectType::ENEMY) {
		isdead = true;
	}
	else if (otherType == ObjectType::ITEM) {
		// 아이템의 색상에 따라 다른 효과
		if (pOther->GetColor() == RGB(255, 0, 255)) { // 핑크색: 탄환 증가
			bulletCount += 5;
			if (bulletCount > 20) bulletCount = 20; // 최대 총알 수 제한
		}
		else if (pOther->GetColor() == RGB(0, 255, 255)) { // 시안색: 속도 증가
			MoveSpeed += 0.1f;
			ShootSpeed += 5.0f;
		}
	}
	else if (otherType == ObjectType::WALL) {
		// 벽과 충돌 시 이동하기 이전 위치로 플레이어와 카메라 롤백
		CGameObject::SetPosition(m_PrevPosition.x, m_PrevPosition.y, m_PrevPosition.z);
		if (m_pCamera) {
			m_pCamera->SetPosition(m_PrevPosition.x, m_PrevPosition.y, m_PrevPosition.z);
		}
		
		// 바운딩 박스를 즉시 갱신시키기 위해 WorldMatrix를 다시 계산해 주어야 추가 충돌 로직이 정상 작동합니다.
		SetWorldMatrix();
	}
}