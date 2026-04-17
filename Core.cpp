#include "Core.h"
#include "PlayScene.h"
#include "TitleScene.h"

Core::Core() {
	m_hInstance = nullptr;
	m_hWnd = nullptr;
	m_hDCFrameBuffer = nullptr;
	m_hBitmapFrameBuffer = nullptr;
	OldCursorPos = { 0, 0 };
}

Core::~Core() {}

void Core::OnCreate(HINSTANCE hInstance, HWND hWnd) {
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	::GetClientRect(m_hWnd, &m_ScreenRect);
	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	
	// 기존 CreateCompatibleBitmap 대신 CreateDIBSection 사용
	int width = m_ScreenRect.right - m_ScreenRect.left;
	int height = m_ScreenRect.bottom - m_ScreenRect.top;

	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // Top-Down 렌더링
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;    // 32비트 색상 (DWORD)
	bmi.bmiHeader.biCompression = BI_RGB;

	// m_pPixelBuffer에 픽셀 배열의 시작 주소가 담기게 됩니다.
	m_hBitmapFrameBuffer = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&m_pPixelBuffer, NULL, 0);

	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);

	BuildObjects();

	_tcscpy_s(m_pszFrameRate, _T("FabProject ("));
}

void Core::OnDestroy() {
	ReleaseObjects();
	m_hInstance = nullptr;
	m_hWnd = nullptr;
}

void Core::FrameAdvance() {
	m_GameTimer.Tick(60.f);

	Input();
	
	AnimateObjects();

	ClearScreen();
	
	m_SceneManager.Rendering(m_hDCFrameBuffer, *m_Camera);

	DrawScreen();

	m_GameTimer.PrintFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void Core::Input() {
	static UCHAR pKeyBuffer[256];
	if (::GetKeyboardState(pKeyBuffer)) {
		int dir = 0;
		if (pKeyBuffer['W'] & 0xF0) dir = 1;
		if (pKeyBuffer['S'] & 0xF0) dir = -1;
		if (pKeyBuffer['A'] & 0xF0) dir = -2;
		if (pKeyBuffer['D'] & 0xF0) dir = 2;
		m_Player->Move(dir);
	}

	if (GetCapture() == m_hWnd) {
		//SetCursor(NULL);	// 커서 숨기기
		POINT CursorPos;
		GetCursorPos(&CursorPos); // 현재 커서 위치 가져오기
		float cxMouseDelta = (float)(CursorPos.x - OldCursorPos.x) / 3.0f; // 마우스 이동량을 3으로 나누어 회전 속도 조절
		float cyMouseDelta = (float)(CursorPos.y - OldCursorPos.y) / 3.0f;
		SetCursorPos(OldCursorPos.x, OldCursorPos.y); // 커서를 이전 위치로 되돌리기
		
		if (cxMouseDelta || cyMouseDelta) {
			m_Player->Rotate(0.0f, cxMouseDelta, 0.0f); // 마우스의 X 이동량을 플레이어의 Y축 회전에 적용
		}
	}
}

void Core::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&OldCursorPos);
		break;
	case WM_LBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void Core::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_SHIFT:
			if (auto bullet = m_Player->Fire()) m_SceneManager.AddObject(bullet);
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Core::BuildObjects() {
	m_Camera = std::make_unique<CCamera>();
	m_Camera->SetCamera();

	m_Player = std::make_unique<CPlayer>();
	m_Player->SetCamera(m_Camera.get());
	m_Player->SetPosition(0.f, 0.f, 0.f);
	m_Player->SetRotation(0.f, 0.f, 0.f);

	m_SceneManager.Init();
}

void Core::ReleaseObjects() {}

void Core::AnimateObjects() {
	float deltatime = m_GameTimer.GetDeltatime();
	m_SceneManager.Animation(deltatime);
}

void Core::DrawScreen() {
	HDC hDC = ::GetDC(m_hWnd);
	
	::BitBlt(hDC, m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right - m_ScreenRect.left,
		m_ScreenRect.bottom - m_ScreenRect.top, m_hDCFrameBuffer, m_ScreenRect.left, m_ScreenRect.top, SRCCOPY);
	
	::ReleaseDC(m_hWnd, hDC);
}

void Core::ClearScreen() {
	COLORREF color = RGB(255, 255, 255);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, color);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(color);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);

	::Rectangle(m_hDCFrameBuffer, m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right, m_ScreenRect.bottom);
	
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}