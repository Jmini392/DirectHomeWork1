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

	void AddGameObject(std::shared_ptr<CGameObject> pObj) { m_GameObjects.push_back(pObj); }
private:
	std::vector<std::shared_ptr<CGameObject>> m_GameObjects;
};