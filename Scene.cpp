#include "Scene.h"
#include "TitleScene.h"

void CSceneManager::Init() {
	std::unique_ptr<CTitleScene> pTitleScene = std::make_unique<CTitleScene>();
	ChangeScene(std::move(pTitleScene));
}

void CSceneManager::ChangeScene(std::unique_ptr<CScene> newScene) {
	CurrentScene = std::move(newScene);
	if (CurrentScene) CurrentScene->BuildObjects();
}

void CSceneManager::Animation(float time) {
	if (CurrentScene) CurrentScene->AnimateObjects(time);

	std::unique_ptr<CScene> nextScene = CurrentScene->GetNextScene();
	if (nextScene != nullptr) {
		ChangeScene(std::move(nextScene));
	}
}

void CSceneManager::Rendering(HDC hDC, CCamera& camera) {
	if (CurrentScene) CurrentScene->DrawObjects(hDC, camera);
}

void CSceneManager::AddObject(std::shared_ptr<CGameObject> pObj) {
	if (CurrentScene) CurrentScene->AddGameObject(pObj);
}