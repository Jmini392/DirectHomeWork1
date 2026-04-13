#include "Scene.h"

void CScene::BuildObjects() {
	// 게임 객체 생성
	CMesh* pCubeMesh = new CCubeMesh(4.f, 4.f, 4.f);
	CMesh* pObjMesh	= new CObjMesh("Model/Mario.obj");
	
	CGameObject pGameObject[2];
	pGameObject[0].SetMesh(pCubeMesh);
	pGameObject[1].SetMesh(pObjMesh);
	//pGameObject[1].SetMesh(pCubeMesh);

	pGameObject[0].SetPosition(0.f, 0.f, 0.f);
	pGameObject[0].SetRotation(0.f, 0.f, 0.f);
	pGameObject[0].SetColor(RGB(255, 0, 255));

	pGameObject[1].SetPosition(5.f, 0.f, 0.f);
	pGameObject[1].SetRotation(0.f, 0.f, 0.f);
	pGameObject[1].SetColor(RGB(0, 255, 255));

	m_GameObjects.push_back(pGameObject[0]);
	m_GameObjects.push_back(pGameObject[1]);
}

void CScene::ReleaseObjects() {
	for (auto& gameObject : m_GameObjects) {
		delete gameObject.GetMesh();
	}
	m_GameObjects.clear(); // 게임 객체 리스트 초기화
}

void CScene::AnimateObjects(float time) {
	for (auto& gameObject : m_GameObjects) {
		gameObject.Animate(time); // 각 게임 객체의 애니메이션 업데이트
	}
}

void CScene::DrawObjects(HDC hDC, CCamera& camera) {
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

	// 오브젝트별 월드 행렬 업데이트 및 그리기
	for (auto& gameObject : m_GameObjects) {		
		gameObject.SetWorldMatrix();
		mpipeline.SetWorldMatrix(gameObject.GetWorldMatrix());
		gameObject.Draw(hDC, mpipeline); // 각 게임 객체를 그립니다.
	}
}