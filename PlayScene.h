#pragma once
#include "PCH.h"
#include "Scene.h"
#include "PipeLine.h"
#include "Camera.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"

class CPlayScene : public CScene {
public:
	CPlayScene() {}
	virtual ~CPlayScene() {}

	void Enter();
	void Exit();

	void Input();
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects();
	void AnimateObjects(float time);
	void DrawObjects(HDC hDC) override;

	void AddGameObject(std::shared_ptr<CGameObject> pObj) { m_GameObjects.push_back(pObj); }
	SceneType GetNextScene() override;
private:
	std::shared_ptr<CPlayer> m_Player;
	std::unique_ptr<CCamera> m_Camera;
	std::vector<std::shared_ptr<CGameObject>> m_GameObjects;
	
	POINT OldCursorPos;
	bool isSceneChanged = false;
	bool isMouseCaptured = true; // 마우스 캡처(회전 모드) 상태 변수 추가
	float itemSpawnTimer = 0.0f; // 아이템 생성 타이머
};