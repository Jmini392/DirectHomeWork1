#include "TitleScene.h"
#include "PlayScene.h"

void CTitleScene::Enter() {
	elapsedTime = 0.0f;
	isSceneChanged = false;
}

void CTitleScene::Exit() {
	// 플레이 씬으로 전환
	isSceneChanged = true;
}

void CTitleScene::BuildObjects() {
	// 타이틀 씬에서는 간단히 배경을 그리는 정도로 구현할 수 있습니다.
	// 예를 들어, 전체 화면을 채우는 사각형을 그릴 수 있습니다.
}

void CTitleScene::AnimateObjects(float time) {
	elapsedTime += time;

	if (elapsedTime >= 3.0f) {
		Exit();
	}
}

void CTitleScene::DrawObjects(HDC hDC, CCamera& camera) {
	RECT rect;
	GetClipBox(hDC, &rect);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 128)); // 어두운 파란색 배경
	FillRect(hDC, &rect, hBrush);
	// 타이틀 텍스트 그리기
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkMode(hDC, TRANSPARENT);
	HFONT hFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	DrawText(hDC, _T("My Game Title"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
}

std::unique_ptr<CScene> CTitleScene::GetNextScene() {
	if (isSceneChanged) {
		isSceneChanged = false;
		return std::make_unique<CPlayScene>();
	}
	return nullptr;
}