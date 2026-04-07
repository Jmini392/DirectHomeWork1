#include "PCH.h"
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
	
	DrawObjects();

	DrawScreen();
}

void Core::OnProsessing() {}

void Core::BuildObjects() {
	camera = new CCamera();
	camera->SetPosition(0.f, 0.f, -10.f);

	m_pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	m_pCubeMesh->MeshColor = RGB(255, 0, 0); // 빨간색으로 설정
}

void Core::ReleaseObjects() {
	if (m_pCubeMesh) {
		delete m_pCubeMesh;
		m_pCubeMesh = nullptr;
	}

	if (camera) {
		delete camera;
		camera = nullptr;
	}
}

void Core::AnimateObjects() {}

void Core::DrawObjects() {
	if (m_pCubeMesh) {
		m_pCubeMesh->Draw(m_hDCFrameBuffer, camera);
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