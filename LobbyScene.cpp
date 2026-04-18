#include "LobbyScene.h"

void CLobbyScene::Enter() {
	isSceneChanged = false;

	BuildObjects();
}

void CLobbyScene::Exit() {
	// 플레이 씬으로 전환
	isSceneChanged = true;
}

void CLobbyScene::BuildObjects() {}

void CLobbyScene::AnimateObjects(float time) {}

void CLobbyScene::DrawObjects(HDC hDC) {
	RECT rect;
	GetClipBox(hDC, &rect);

	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hDC, &rect, hBrush);

	SetTextColor(hDC, RGB(0, 0, 0));
	SetBkMode(hDC, TRANSPARENT);
	HFONT hFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
	HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
	DrawText(hDC, _T("Press 'Space' to play"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	DeleteObject(hBrush);
}

SceneType CLobbyScene::GetNextScene() {
	if (isSceneChanged) return SceneType::PLAY;
	return SceneType::NONE;
}

void CLobbyScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_SPACE:
			Exit();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CLobbyScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		break;
	default:
		break;
	}
}