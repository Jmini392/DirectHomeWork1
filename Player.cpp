#include "Player.h"
#include "Scene.h"

void CPlayer::SetPosition(float x, float y, float z) {
	position = XMFLOAT3(x, y, z);
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
	std::shared_ptr<CMesh> pSphereMesh = std::make_shared<CObjMesh>("Model/Sphere.obj");
	
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
	pBullet->SetSpeed(15.f); 

	// 플레이어 위치를 기준으로 총알의 생성 위치를 정합니다. 
	XMFLOAT3 pos = GetPosition();
	float spawnOffset = 2.0f; // 플레이어의 중심에서 앞쪽으로 떨어뜨릴 거리 지정
	
	// 현재 위치에 전방 벡터 * 일정 거리
	pBullet->SetPosition(pos.x + (forward.x * spawnOffset),pos.y, pos.z + (forward.z * spawnOffset));

	// 총알 객체 반환
	return pBullet;
}