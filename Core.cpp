#include "Core.h"

Core::Core() {
	m_hInstance = nullptr;
	m_hWnd = nullptr;
}

Core::~Core() {}

void Core::OnCreate(HINSTANCE hInstance, HWND hWnd) {
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	::GetClientRect(m_hWnd, &m_ScreenRect);
	HDC hDC = ::GetDC(m_hWnd);

	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_ScreenRect.right - m_ScreenRect.left, m_ScreenRect.bottom - m_ScreenRect.top);

	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
	
	BuildObjects();
}

void Core::OnDestroy() {
	ReleaseObjects();
	m_hInstance = nullptr;
	m_hWnd = nullptr;
}

void Core::FrameAdvance() {
	OnProsessing();
	
	AnimateObjects();

	ClearScreen();
	
	if (m_pScene && m_pCamera) {
		m_pScene->DrawObjects(m_hDCFrameBuffer, *m_pCamera);
	}

	DrawScreen();
}

void Core::OnProsessing() {
	static UCHAR pKeyBuffer[256];
	if (::GetKeyboardState(pKeyBuffer)) {
		float cxKeyDelta = 0.0f, cyKeyDelta = 0.0f, czKeyDelta = 0.0f;
		if (pKeyBuffer['W'] & 0xF0) czKeyDelta = +0.125f;
		if (pKeyBuffer['S'] & 0xF0) czKeyDelta = -0.125f;
		if (pKeyBuffer['A'] & 0xF0) cxKeyDelta = -0.125f;
		if (pKeyBuffer['D'] & 0xF0) cxKeyDelta = +0.125f;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) cyKeyDelta = +0.125f;
		if (pKeyBuffer[VK_NEXT] & 0xF0) cyKeyDelta = -0.125f;
		m_pPlayer->Move(cxKeyDelta, cyKeyDelta, czKeyDelta);
	}
}

void Core::BuildObjects() {
	m_pCamera = new CCamera();
	m_pCamera->SetCamera();

	m_pPlayer = new CPlayer();
	m_pPlayer->SetCamera(m_pCamera);
	m_pPlayer->SetPosition(0.f, 0.f, -10.f);
	m_pPlayer->SetRotation(0.f, 0.f, 0.f);

	m_pScene = new CScene();
	m_pScene->BuildObjects();
}

void Core::ReleaseObjects() {
	if (m_pPlayer) delete m_pPlayer;

	if (m_pScene) {
		m_pScene->ReleaseObjects();
		delete m_pScene;
		m_pScene = nullptr;
	}	
}

void Core::AnimateObjects() {
	if (m_pScene) {
		m_pScene->AnimateObjects();
	}
}

void Core::DrawScreen() {
	HDC hDC = ::GetDC(m_hWnd);
	
	::BitBlt(hDC, m_ScreenRect.left, m_ScreenRect.top, m_ScreenRect.right - m_ScreenRect.left,
		m_ScreenRect.bottom - m_ScreenRect.top, m_hDCFrameBuffer, m_ScreenRect.left, m_ScreenRect.top, SRCCOPY);
	
	::ReleaseDC(m_hWnd, hDC);
}

void Core::ClearScreen() {
	auto color = RGB(255, 255, 255);
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