#include "TitleScene.h"

CTitleScene::~CTitleScene() {
	if (m_hTitleImage) {
		DeleteObject(m_hTitleImage);
		m_hTitleImage = NULL;
	}
}

void CTitleScene::Enter() {
	elapsedTime = 0.0f;
	isSceneChanged = false;
	
	m_hTitleImage = (HBITMAP)LoadImage(NULL, _T("title.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	BuildObjects();
}

void CTitleScene::Exit() {
	// 씬전환
	isSceneChanged = true;
}

void CTitleScene::BuildObjects() {}

void CTitleScene::AnimateObjects(float time) {
	elapsedTime += time;

	if (elapsedTime >= 3.0f) Exit();
}

void CTitleScene::DrawObjects(HDC hDC) {
	RECT rect;
	GetClipBox(hDC, &rect);

	if (m_hTitleImage != NULL) {
		// 메모리 DC 생성
		HDC hMemDC = CreateCompatibleDC(hDC);

		// 메모리 DC에 로드한 비트맵 장착
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hTitleImage);

		// 이미지의 원래 크기(너비, 높이) 정보 얻어오기
		BITMAP bmp;
		GetObject(m_hTitleImage, sizeof(BITMAP), &bmp);

		// 출력 (화면 전체에 늘릴 경우 StretchBlt, 원래 크기대로면 BitBlt)
		// 여기서는 창 크기(rect)에 꽉 차게 늘려서 그립니다 (StretchBlt)
		StretchBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
			hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

		// 자원 정리
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	else {
		// 이미지가 로드 실패했다면 예비로 기존 파란색 화면 렌더링 유지
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 128));
		FillRect(hDC, &rect, hBrush);

		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkMode(hDC, TRANSPARENT);
		HFONT hFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
		HFONT hOldFont = (HFONT)SelectObject(hDC, hFont);
		DrawText(hDC, _T("Image Load Failed\n(title.bmp needs to be in folder)"), -1, &rect, DT_CENTER | DT_VCENTER);
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);
		DeleteObject(hBrush);
	}
}

SceneType CTitleScene::GetNextScene() {
	if (isSceneChanged) return SceneType::LOBBY;
	return SceneType::NONE;
}

void CTitleScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CTitleScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}