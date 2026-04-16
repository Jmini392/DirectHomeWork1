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
	XMFLOAT3 moveVec = direction; // 앞뒤 이동을 위한 기본 전방 벡터 유지
	int moveSign = dir;           // 앞(1), 뒤(-1) 곱셈용 변수
	if (dir == 2 || dir == -2) { // 좌우 이동 (&&가 아닌 || 사용)
		// 전방 벡터를 Y축 기준 90도 회전하여 우측 벡터를 임시로 생성 (direction 원본 훼손 방지)
		moveVec = XMFLOAT3(-direction.z, direction.y, direction.x);		
		// D(-2)를 누르면 오른쪽(+1), A(2)를 누르면 왼쪽(-1)으로 부호 지정
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

void CPlayer::Fire() {
	if (!m_pCamera || !m_pScene) return;

	// 총알 객체를 동적으로 생성
	std::shared_ptr<CMesh> pCubeMesh = std::make_shared<CCubeMesh>(0.5f, 0.5f, 0.5f);
	
	std::shared_ptr<CBullet> pBullet = std::make_shared<CBullet>();
	pBullet->SetMesh(pCubeMesh);
	pBullet->SetColor(RGB(255, 0, 0));

	// 1. 발사 방향(Forward)을 먼저 계산합니다.
	XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	XMFLOAT3 forward = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);
	
	XMVECTOR vForward = XMLoadFloat3(&forward);
	vForward = XMVector3Normalize(vForward);
	XMStoreFloat3(&forward, vForward);

	pBullet->SetDirection(forward);
	pBullet->SetSpeed(15.f); 

	// 2. 플레이어 위치를 기준으로 총알의 생성 위치를 정합니다. 
	XMFLOAT3 pos = GetPosition();
	float spawnOffset = 2.0f; // ★ 플레이어의 중심에서 앞쪽으로 떨어뜨릴 거리 지정
	
	// 현재 위치에 전방 벡터 * 일정 거리를 더해서 앞에서 뿅 나오게 만듭니다.
	pBullet->SetPosition(
		pos.x + (forward.x * spawnOffset), 
		pos.y - 1.f,                       // 총구 역할을 위해 살짝 아래쪽에서 발사되도록 유지
		pos.z + (forward.z * spawnOffset)
	);

	// 씬에 포인터 등록
	m_pScene->AddGameObject(pBullet);
}