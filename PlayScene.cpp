#include "PlayScene.h"

void CPlayScene::Enter() {
	BuildObjects();

	// 씬 진입 시 초기 마우스 위치 셋업
	::GetCursorPos(&OldCursorPos);
	ShowCursor(FALSE); // 커서 숨기기
	isMouseCaptured = true; // 마우스 캡처(회전 모드) 활성화
}

void CPlayScene::Exit() {
	ShowCursor(TRUE); // 커서 보이기
	isSceneChanged = true;
}

void CPlayScene::BuildObjects() {
	m_Camera = std::make_unique<CCamera>();
	m_Camera->SetCamera();

	m_Player = std::make_unique<CPlayer>();
	m_Player->SetCamera(m_Camera.get());
	m_Player->SetPosition(0.f, 0.f, 0.f);
	m_Player->SetRotation(0.f, 0.f, 0.f);

	std::shared_ptr<CItem> pGameObject1 = std::make_shared<CItem>();
	pGameObject1->SetPosition(10.f, -2.f, 20.f);

	m_GameObjects.push_back(pGameObject1);

	std::shared_ptr<CItem> pGameObject2 = std::make_shared<CItem>();
	pGameObject2->SetPosition(35.f, -2.f, 10.f);

	m_GameObjects.push_back(pGameObject2);

	float wallWidth = 45.0f;  // 성벽의 너비
	float radius = 150.0f;    // 중심으로부터의 거리
	// 원의 둘레(2 * pi * r)를 벽의 너비로 나눕니다.
	int numWalls = static_cast<int>((2.0f * PI * radius) / (wallWidth - 0.5f));
	for (int i = 0; i < numWalls; ++i) {
		float angle = (2.0f * PI / numWalls) * i;
		float x = radius * cosf(angle);
		float z = radius * sinf(angle);

		std::shared_ptr<CWall>pWall = std::make_shared<CWall>();
		pWall->SetPosition(x, 0.f, z);
		pWall->SetRotation(0.f, -angle, 0.f);
		if (i % 3 == 0) pWall->SetColor(RGB(150, 150, 150));
		else if(i % 3 == 1) pWall->SetColor(RGB(120, 120, 120));
		else pWall->SetColor(RGB(100, 100, 100));

		m_GameObjects.push_back(pWall);
	}

	// 바닥 추가
	float tileSize = 20.0f; // 타일 한 칸의 크기
	int gridSize = 20; // 10x10 격자 (총 크기: 200x200)
	int halfGrid = gridSize / 2;
	for (int i = -halfGrid; i < halfGrid; ++i) {
		for (int j = -halfGrid; j < halfGrid; ++j) {
			// 격자의 중심을 기준으로 타일 배치
			float posX = (i * tileSize) + (tileSize * 0.5f);
			float posZ = (j * tileSize) + (tileSize * 0.5f);

			std::shared_ptr<CFloor> pFloorObj = std::make_shared<CFloor>();
			pFloorObj->SetPosition(posX, -30.f, posZ);
			if ((i + j) % 2 == 0) pFloorObj->SetColor(RGB(255, 100, 100));
			else pFloorObj->SetColor(RGB(215, 60, 60));

			m_GameObjects.push_back(pFloorObj);
		}
	}

	// 적 객체 추가
	std::shared_ptr<CEnemy> pEnemy = std::make_shared<CEnemy>();
	pEnemy->SetPosition(0.f, -2.f, 50.f);
	pEnemy->SetColor(RGB(0, 255, 0));

	m_GameObjects.push_back(pEnemy);
}

void CPlayScene::AnimateObjects(float time) {
	// 벡터(리스트) 순회 중 삭제를 안전하게 하기 위해 iterator(반복자)를 사용합니다.
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ) {
		(*it)->Animate(time);
		if ((*it)->isdead) it = m_GameObjects.erase(it); // isdead가 true인 경우 삭제
		else ++it; // 다음 요소로 이동
	}
}

void CPlayScene::DrawObjects(HDC hDC) {
	//-----------------------------------------------------------------------------
	// 오브젝트 그리기
	CPipeLine mpipeline;

	// 코어(Core) 수정 없이 HDC에 바인딩된 DIBSection(픽셀 버퍼) 의 포인터를 얻어와 파이프라인에 연결합니다.
	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);
	DIBSECTION ds;
	if (::GetObject(hBitmap, sizeof(DIBSECTION), &ds) == sizeof(DIBSECTION)) {
		mpipeline.SetPixelBuffer((DWORD*)ds.dsBm.bmBits);
	}

	// --- Z-버퍼 초기화 (PCH.h에 있는 FRAME_BUFFER_WIDTH/HEIGHT 사용) ---
	mpipeline.InitZBuffer(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	mpipeline.ClearZBuffer();

	// 카메라 뷰, 투영 행렬 세팅
	m_Camera->SetViewMatrix();
	m_Camera->SetProjMatrix();
	m_Camera->SetViewportMatrix();
	mpipeline.SetViewMatrix(m_Camera->GetViewMatrix());
	mpipeline.SetProjMatrix(m_Camera->GetProjMatrix());
	mpipeline.SetViewportMatrix(m_Camera->GetViewportMatrix());

	for (auto pObj : m_GameObjects) {
		pObj->SetWorldMatrix();
		mpipeline.SetWorldMatrix(pObj->GetWorldMatrix());
		// 게임 오브젝트의 색상과 메시를 가져와서 파이프라인에 전달하여 그립니다.
		if (pObj->GetMesh()) {
			COLORREF color = pObj->GetColor();
			CMesh* mesh = pObj->GetMesh().get();
			mpipeline.DrawObject(hDC, mesh, color);
		}
	}
	//-----------------------------------------------------------------------------
	// UI 그리기
}

SceneType CPlayScene::GetNextScene() {
	if (isSceneChanged) return SceneType::LOBBY;
	return SceneType::NONE;
}

void CPlayScene::Input() {
	// 커서가 숨겨져있으면
	if (isMouseCaptured) {
		static UCHAR pKeyBuffer[256];
		if (::GetKeyboardState(pKeyBuffer)) {
			int dir = 0;
			if (pKeyBuffer['W'] & 0xF0) dir = 1;
			if (pKeyBuffer['S'] & 0xF0) dir = -1;
			if (pKeyBuffer['A'] & 0xF0) dir = -2;
			if (pKeyBuffer['D'] & 0xF0) dir = 2;
			m_Player->Move(dir);
		}

		POINT CursorPos;
		GetCursorPos(&CursorPos); // 현재 커서 위치 가져오기
		float cxMouseDelta = (float)(CursorPos.x - OldCursorPos.x) / 3.0f; // 마우스 이동량을 3으로 나누어 회전 속도 조절
		float cyMouseDelta = (float)(CursorPos.y - OldCursorPos.y) / 3.0f;
		
		// 마우스의 이동이 있었을 때만 회전 및 커서 원위치 (무한 회전 방지)
		if (cxMouseDelta != 0.0f || cyMouseDelta != 0.0f) {
			m_Player->Rotate(0.0f, cxMouseDelta, 0.0f); // 마우스의 X 이동량을 플레이어의 Y축 회전에 적용
			SetCursorPos(OldCursorPos.x, OldCursorPos.y); // 커서를 이전 위치로 되돌리기
		}
	}
}

void CPlayScene::KeyboardProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case 'R':
			Exit();
			break;
		case VK_SHIFT:
			if (isMouseCaptured) if (auto bullet = m_Player->Fire()) AddGameObject(bullet);
			break;
		case VK_ESCAPE:
			if (isMouseCaptured) {
				ShowCursor(TRUE); // 커서 보이기
				isMouseCaptured = false; // 마우스 캡처 비활성화
			}
			else {
				ShowCursor(FALSE); // 커서 숨기기
				isMouseCaptured = true; // 마우스 캡처 활성화
				::GetCursorPos(&OldCursorPos); // 점프 방지를 위해 기준 위치 재설정
			}			
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CPlayScene::MouseProcessing(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
		if (!isMouseCaptured) {
			::ShowCursor(FALSE);
			isMouseCaptured = true;
			::GetCursorPos(&OldCursorPos); // 점프 방지를 위해 기준 위치 재설정
		}
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}