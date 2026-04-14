#pragma once
#include "PCH.h"
#include "GameObject.h"
#include "Camera.h"

class CScene; // 전방 선언

class CPlayer : public CGameObject {
public:
	CPlayer() {}
	virtual ~CPlayer() {}

	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }
	void SetScene(CScene* pScene) { m_pScene = pScene; }

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(int dir);
	void Rotate(float x, float y, float z);

	void Fire();
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
	CScene* m_pScene = nullptr;
	XMFLOAT3 direction; // 플레이어가 바라보는 방향
	XMFLOAT3 position; // 플레이어의 위치
	float MoveSpeed = 0.1f; // 이동 속도
};