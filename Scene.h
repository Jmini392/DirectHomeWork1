#pragma once
#include "PCH.h"
#include "Camera.h"
#include "Time.h"

// 기본 씬 인터페이스 (추상 클래스)
class CScene {
public:
	CScene() {}
	virtual ~CScene() {}

	virtual void Enter() {}
	virtual void Exit() {}

	virtual void BuildObjects() {}
	virtual void AnimateObjects(float time) {}
	virtual void DrawObjects(HDC hDC, CCamera& camera) {}
	virtual void AddGameObject(std::shared_ptr<class CGameObject> pObj) {}

	virtual std::unique_ptr<CScene> GetNextScene() { return nullptr; }
};

// 씬 매니저 일반 클래스 (Core가 소유하게 됨)
class CSceneManager {
public:
	CSceneManager() {}
	~CSceneManager() {}

	// 처음 시작할 씬 설정
	void Init();
	// 새로운 씬으로 전환
	void ChangeScene(std::unique_ptr<CScene> newScene);
	// 현재 씬 업데이트
	void Animation(float time);
	// 현재 씬 렌더링
	void Rendering(HDC hDC, CCamera& camera);
	// 현재 씬에 객체를 추가하는 함수
	void AddObject(std::shared_ptr<class CGameObject> pObj);
private:
	std::unique_ptr<CScene> CurrentScene;
};