#include "Player.h"

void CPlayer::SetPosition(float x, float y, float z) {
	CGameObject::SetPosition(x, y, z);
	if (m_pCamera) m_pCamera->SetPosition(x, y, z);
}

void CPlayer::SetRotation(float x, float y, float z) {
	CGameObject::SetRotation(x, y, z);
	if (m_pCamera) m_pCamera->SetRotation(x, y, z);
}

void CPlayer::Move(DWORD dir) {
	// 카메라의 뷰행렬에서 방향 벡터를 가져온다. 
	XMFLOAT4X4 viewMatrix = m_pCamera->GetViewMatrix();
	XMFLOAT3 right = XMFLOAT3(viewMatrix._11, viewMatrix._21, viewMatrix._31);
	XMFLOAT3 forward = XMFLOAT3(viewMatrix._13, viewMatrix._23, viewMatrix._33);

	if (dir == 0) {
		CGameObject::Move(forward.x * MoveSpeed, forward.y * MoveSpeed, forward.z * MoveSpeed);
		if (m_pCamera) m_pCamera->Move(forward.x * MoveSpeed, forward.y * MoveSpeed, forward.z * MoveSpeed);
	}
	else if (dir == 1) {
		CGameObject::Move(-forward.x * MoveSpeed, -forward.y * MoveSpeed, -forward.z * MoveSpeed);
		if (m_pCamera) m_pCamera->Move(-forward.x * MoveSpeed, -forward.y * MoveSpeed, -forward.z * MoveSpeed);
	}
	else if (dir == 2) {
		CGameObject::Move(-right.x * MoveSpeed, -right.y * MoveSpeed, -right.z * MoveSpeed);
		if (m_pCamera) m_pCamera->Move(-right.x * MoveSpeed, -right.y * MoveSpeed, -right.z * MoveSpeed);
	}
	else if (dir == 3) {
		CGameObject::Move(right.x * MoveSpeed, right.y * MoveSpeed, right.z * MoveSpeed);
		if (m_pCamera) m_pCamera->Move(right.x * MoveSpeed, right.y * MoveSpeed, right.z * MoveSpeed);
	}
}

void CPlayer::Rotate(float x, float y, float z) {
	CGameObject::Rotate(x, y, z);
	if (m_pCamera) m_pCamera->Rotate(x, y, z);
}