#pragma once
#include "PCH.h"
#include "GameObject.h"
#include "Camera.h"

class CScene;

class CPlayer : public CGameObject {
public:
	CPlayer() {}
	virtual ~CPlayer() {}

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(int dir);
	void Rotate(float x, float y, float z);

	std::shared_ptr<CGameObject> Fire();
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
	XMFLOAT3 direction = { 0.0f, 0.0f, 1.0f }; // 플레이어가 바라보는 방향
	float MoveSpeed = 0.1f; // 이동 속도
};