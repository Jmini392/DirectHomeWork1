#pragma once
#include "Mesh.h"
#include "Camera.h"

class Core {
public:
	// 생성과 소멸
	Core();
	~Core();
	
	// 초기화
	void OnCreate(HINSTANCE hInstance, HWND hWnd);
	// 종료
	void OnDestroy();
	// 프레임 진행 (반복)
	void FrameAdvance();

	// 키 입력
	void OnProsessing();

	// 객체 생성과 소멸
	void BuildObjects();
	void ReleaseObjects();

	// 객체 애니메이션
	void AnimateObjects();

	// 객체 그리기
	void DrawObjects();

	// 화면 그리기
	void DrawScreen();

	// 화면 초기화
	void ClearScreen();

	CCamera* camera = nullptr;
private:
	// 윈도우 핸들과 인스턴스 핸들
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	// 화면 크기
	RECT m_ScreenRect;

	// 프레임 버퍼
	HDC m_hDCFrameBuffer;
	HBITMAP m_hBitmapFrameBuffer;

	// 카메라 객체
	//CCamera* camera = nullptr;

	// 메쉬 객체
	CMesh* m_pCubeMesh = nullptr;
};