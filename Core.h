#pragma once
#include "PCH.h"
#include "Camera.h"
#include "Scene.h"
#include "Player.h"
#include "Time.h"

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
	void Input();
	// 키보드, 마우스 입력
	void KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 객체 생성과 소멸
	void BuildObjects();
	void ReleaseObjects();

	// 객체 애니메이션
	void AnimateObjects();

	// 화면 그리기
	void DrawScreen();

	// 화면 초기화
	void ClearScreen();
private:
	// 윈도우 핸들과 인스턴스 핸들
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	// 화면 크기
	RECT m_ScreenRect;

	// 프레임 버퍼
	HDC m_hDCFrameBuffer;
	HBITMAP m_hBitmapFrameBuffer;
	// 컬러 버퍼 화면 픽셀 데이터를 직접 조작하기 위한 1차원 포인터 배열 추가
	DWORD* m_pPixelBuffer = nullptr; 

	// 카메라 객체
	std::unique_ptr<CCamera> m_Camera;

	// 게임 씬 객체
	CSceneManager m_SceneManager;
	//std::unique_ptr<CScene> m_Scene;

	// 플레이어 객체
	std::unique_ptr<CPlayer> m_Player;

	// 프레임 레이트 문자열 버퍼
	TCHAR m_pszFrameRate[50] = { 0 };
	// 게임 타이머 객체
	CTime m_GameTimer;

	// 이전 프레임의 마우스 커서 위치 저장 변수 추가
	POINT OldCursorPos;
};