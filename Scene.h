#pragma once
#include "PCH.h"
#include "Camera.h"
#include "GameObject.h"
#include "Mesh.h"
#include "PipeLine.h"

class CScene {
public:
	CScene() {}
	virtual ~CScene() {}

	void BuildObjects();
	void ReleaseObjects();
	void AnimateObjects(float time);
	void DrawObjects(HDC hDC, CCamera& camera);
private:
	// 게임 객체들 저장 배열
	std::vector<CGameObject> m_GameObjects;
};