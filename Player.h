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
	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);
private:
	CCamera* m_pCamera = nullptr; // 플레이어의 카메라
};