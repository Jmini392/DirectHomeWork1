#pragma once
#include "PCH.h"
#include "Scene.h"
#include "Camera.h"

class CTitleScene : public CScene {
public:
	CTitleScene() {}
	virtual ~CTitleScene() {}

	void Enter();
	void Exit();

	void BuildObjects();
	void AnimateObjects(float time);
	void DrawObjects(HDC hDC, CCamera& camera);

	std::unique_ptr<CScene> GetNextScene() override;
private:
	float elapsedTime = 0.0f;
	bool isSceneChanged = false;
};