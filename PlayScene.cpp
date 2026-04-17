#include "PlayScene.h"

void CPlayScene::BuildObjects() {
	// 게임 객체 생성
	std::shared_ptr<CMesh> pCubeMesh = std::make_shared<CCubeMesh>(4.f, 4.f, 4.f);

	std::shared_ptr<CRotate> pGameObject1 = std::make_shared<CRotate>();
	pGameObject1->SetMesh(pCubeMesh);
	pGameObject1->SetPosition(10.f, 0.f, 20.f);
	pGameObject1->SetRotation(0.f, 0.f, 0.f);
	pGameObject1->SetColor(RGB(255, 0, 255));

	std::shared_ptr<CRotate> pGameObject2 = std::make_shared<CRotate>();
	pGameObject2->SetMesh(pCubeMesh);
	pGameObject2->SetPosition(35.f, 0.f, 10.f);
	pGameObject2->SetRotation(0.f, 0.f, 0.f);
	pGameObject2->SetColor(RGB(0, 255, 255));

	m_GameObjects.push_back(pGameObject1);
	m_GameObjects.push_back(pGameObject2);

	float wallWidth = 30.0f;  // 성벽의 너비
	float radius = 100.0f;    // 중심으로부터의 거리

	// 원의 둘레(2 * pi * r)를 벽의 너비로 나눕니다.
	int numWalls = static_cast<int>((2.0f * PI * radius) / (wallWidth - 0.5f));

	std::shared_ptr<CMesh> pWallMesh = std::make_shared<CCubeMesh>(2.0f, 50.f, wallWidth);

	for (int i = 0; i < numWalls; ++i) {
		float angle = (2.0f * PI / numWalls) * i;

		float x = radius * cosf(angle);
		float z = radius * sinf(angle);

		std::shared_ptr<CGameObject> pWall = std::make_shared<CGameObject>();
		pWall->SetMesh(pWallMesh);
		pWall->SetPosition(x, 0.f, z);
		pWall->SetRotation(0.f, -angle, 0.f);
		pWall->SetColor(RGB(150, 150, 150));

		m_GameObjects.push_back(pWall);
	}

	// 바닥 추가
	float tileSize = 20.0f; // 타일 한 칸의 크기
	int gridSize = 20; // 10x10 격자 (총 크기: 200x200)

	std::shared_ptr<CMesh> pFloorMesh = std::make_shared<CCubeMesh>(tileSize, 0.5f, tileSize);

	int halfGrid = gridSize / 2;
	for (int i = -halfGrid; i < halfGrid; ++i) {
		for (int j = -halfGrid; j < halfGrid; ++j) {
			std::shared_ptr<CGameObject> pFloorObj = std::make_shared<CGameObject>();
			pFloorObj->SetMesh(pFloorMesh);

			// 격자의 중심을 기준으로 타일 배치
			float posX = (i * tileSize) + (tileSize * 0.5f);
			float posZ = (j * tileSize) + (tileSize * 0.5f);

			pFloorObj->SetPosition(posX, -30.f, posZ);
			pFloorObj->SetRotation(0.f, 0.f, 0.f);
			if ((i + j) % 2 == 0) pFloorObj->SetColor(RGB(255, 100, 100));
			else pFloorObj->SetColor(RGB(215, 60, 60));

			m_GameObjects.push_back(pFloorObj);
		}
	}
}

void CPlayScene::AnimateObjects(float time) {
	// 벡터(리스트) 순회 중 삭제를 안전하게 하기 위해 iterator(반복자)를 사용합니다.
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ) {
		(*it)->Animate(time);
		if ((*it)->isdead) it = m_GameObjects.erase(it); // isdead가 true인 경우 삭제
		else ++it; // 다음 요소로 이동
	}
}

void CPlayScene::DrawObjects(HDC hDC, CCamera& camera) {
	// 파이프라인 객체생성
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
	camera.SetViewMatrix();
	camera.SetProjMatrix();
	camera.SetViewportMatrix();
	mpipeline.SetViewMatrix(camera.GetViewMatrix());
	mpipeline.SetProjMatrix(camera.GetProjMatrix());
	mpipeline.SetViewportMatrix(camera.GetViewportMatrix());

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
}