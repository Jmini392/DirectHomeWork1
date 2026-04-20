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

	m_Player = std::make_shared<CPlayer>();
	m_Player->SetCamera(m_Camera.get());
	m_Player->SetPosition(0.f, 0.f, 0.f);
	m_Player->SetRotation(0.f, 0.f, 0.f);
	AddGameObject(m_Player);

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
			pFloorObj->SetPosition(posX, -10.f, posZ);
			if ((i + j) % 2 == 0) pFloorObj->SetColor(RGB(255, 100, 100));
			else pFloorObj->SetColor(RGB(215, 60, 60));
			AddGameObject(pFloorObj);
		}
	}

	for (int i = 0; i < 8; ++i) {
		float angle = i * 45.f * (XM_PI / 180.f); // 45도 간격으로 배치
		float x = 170 * cosf(angle);
		float z = 170 * sinf(angle);

		std::shared_ptr<CWall>pWall = std::make_shared<CWall>();
		pWall->SetPosition(x, 10.f, z);
		pWall->SetRotation(0.f, -angle, 0.f);
		if (i % 2 == 0) pWall->SetColor(RGB(150, 150, 150));
		else pWall->SetColor(RGB(100, 100, 100));
		AddGameObject(pWall);
	}	

	// 적 객체 추가
	std::shared_ptr<CEnemy> pEnemy1 = std::make_shared<CEnemy>(0);
	pEnemy1->SetPosition(0.f, 0.f, 50.f);
	pEnemy1->SetTarget(m_Player);
	AddGameObject(pEnemy1);
	std::shared_ptr<CEnemy> pEnemy2 = std::make_shared<CEnemy>(1);
	pEnemy2->SetPosition(50.f, 0.f, 0.f);
	pEnemy2->SetTarget(m_Player);
	AddGameObject(pEnemy2);
	std::shared_ptr<CEnemy> pEnemy3 = std::make_shared<CEnemy>(2);
	pEnemy3->SetPosition(-50.f, 0.f, 0.f);
	pEnemy3->SetTarget(m_Player);
	AddGameObject(pEnemy3);
}

void CPlayScene::AnimateObjects(float time) {
	// 아이템 스폰 (5초마다)
	itemSpawnTimer += time;
	if (itemSpawnTimer >= 5.0f) {
		itemSpawnTimer -= 5.0f; // 타이머 초기화

		// 맵 범위 내에서 랜덤한 위치 생성
		float randomX = (float)FIELD_RANDOM;
		float randomZ = (float)FIELD_RANDOM;

		// 새 아이템 생성 후 리스트에 추가
		std::shared_ptr<CItem> pNewItem = std::make_shared<CItem>();
		pNewItem->SetPosition(randomX, -2.f, randomZ);		
		AddGameObject(pNewItem);
	}

	// 상태 갱신 및 죽은 객체 삭제
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ) {
		(*it)->Animate(time);
		(*it)->SetWorldMatrix(); 

		if ((*it)->isdead) {
			if ((*it)->GetType() == ObjectType::PLAYER) {
				isSceneChanged = true; // 플레이어가 죽으면 씬 전환 플래그 설정
			}
			it = m_GameObjects.erase(it);
		}
		else ++it;
	}

	// 전체 객체 중첩 순회
	for (size_t i = 0; i < m_GameObjects.size(); ++i) {
		auto& obj1 = m_GameObjects[i];

		for (size_t j = i + 1; j < m_GameObjects.size(); ++j) {
			auto& obj2 = m_GameObjects[j];

			// 둘 중 하나라도 소멸 상태면 검사 X
			if (obj1->isdead || obj2->isdead) continue;

			// 필요 없는 충돌은 타입 기반으로 제외 (최적화)
			ObjectType type1 = obj1->GetType();
			ObjectType type2 = obj2->GetType();
			
			// 바닥은 모든 충돌 체크에서 제외 (부하 방지)
			if (type1 == ObjectType::FLOOR || type2 == ObjectType::FLOOR) continue;
			
			// 동족 간의 충돌 무시 (벽끼리, 적끼리, 총알끼리 등)
			if (type1 == type2) continue;

			// 두 객체의 바운딩 박스 교차 검사
			if (obj1->GetWorldBoundingBox().Intersects(obj2->GetWorldBoundingBox())) {
				obj1->OnCollision(obj2);
				obj2->OnCollision(obj1);
			}
		}
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