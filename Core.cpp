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
	m_pCamera = new CCamera();
	m_pCamera->SetPosition(0.f, 0.f, -10.f);
	m_pCamera->SetRotation(0.f, 0.f, 0.f);
	m_pCamera->SetViewMatrix();

	m_pViewport = new CViewport();
	m_pViewport->SetProjMatrix();
	m_pViewport->SetViewportMatrix();
	
	m_pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	m_pGameObject = new CGameObject();
	m_pGameObject->SetMesh(m_pCubeMesh);
}

void Core::ReleaseObjects() {
	if (m_pCubeMesh) {
		delete m_pCubeMesh;
		m_pCubeMesh = nullptr;
	}

	if (m_pCamera) {
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pGameObject) {
		delete m_pGameObject;
		m_pGameObject = nullptr;
	}

	if (m_pViewport) {
		delete m_pViewport;
		m_pViewport = nullptr;
	}
}

void Core::AnimateObjects() {}

void Core::DrawObjects() {
	if (m_pCubeMesh && m_pCamera && m_pViewport) {
		// 1. 카메라(View) 행렬 업데이트 및 파이프라인 설정
		m_pCamera->SetViewMatrix();
		m_Pipeline.SetViewMatrix(m_pCamera->GetViewMatrix());

		// 2. 투영(Proj) 및 뷰포트 행렬 설정
		m_Pipeline.SetProjMatrix(m_pViewport->GetProjMatrix());
		m_Pipeline.SetViewportMatrix(m_pViewport->GetViewportMatrix());

		// 3. 오브젝트별 월드 행렬 업데이트 및 그리기
		m_pGameObject->SetWorldMatrix();
		m_Pipeline.SetWorldMatrix(m_pGameObject->GetWorldMatrix());
	
		m_pGameObject->Draw(m_hDCFrameBuffer, m_Pipeline);
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