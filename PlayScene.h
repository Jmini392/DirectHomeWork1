#pragma once
#include "PCH.h"
#include "Scene.h"
#include "Camera.h"
#include "GameObject.h"
#include "Mesh.h"
#include "PipeLine.h"

class CPlayScene : public CScene {
public:
	CPlayScene() {}
	virtual ~CPlayScene() {}

	void Enter() {}
	void Exit() {}

	void BuildObjects();
	void AnimateObjects(float time);
	void DrawObjects(HDC hDC, CCamera& camera);

	void AddGameObject(std::shared_ptr<CGameObject> pObj) { m_GameObjects.push_back(pObj); }
private:
	std::vector<std::shared_ptr<CGameObject>> m_GameObjects;
};