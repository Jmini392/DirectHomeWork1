#pragma once
#include "PCH.h"
#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject {
public:
	CPlayer() {}
	virtual ~CPlayer() {}

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(DWORD dir);
	void Rotate(float x, float y, float z);

	void Fire();
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
	float MoveSpeed = 0.1f; // 이동 속도
};