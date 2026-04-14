#include "Scene.h"

void CScene::BuildObjects() {
	// 게임 객체 생성
	CMesh* pCubeMesh = new CCubeMesh(4.f, 4.f, 4.f);
	CMesh* pObjMesh = new CObjMesh("Model/Mario.obj");
	
	// 객체도 동적 할당(new)으로 생성
	CGameObject* pGameObject1 = new CGameObject();
	pGameObject1->SetMesh(pCubeMesh);
	pGameObject1->SetPosition(0.f, 0.f, 0.f);
	pGameObject1->SetRotation(0.f, 0.f, 0.f);
	pGameObject1->SetColor(RGB(255, 0, 255));

	CGameObject* pGameObject2 = new CGameObject();
	pGameObject2->SetMesh(pObjMesh);
	pGameObject2->SetPosition(5.f, 0.f, 0.f);
	pGameObject2->SetRotation(0.f, 0.f, 0.f);
	pGameObject2->SetColor(RGB(0, 255, 255));

	m_GameObjects.push_back(pGameObject1);
	m_GameObjects.push_back(pGameObject2);
}

void CScene::ReleaseObjects() {
	for (auto pObj : m_GameObjects) {
		delete pObj->GetMesh(); // 메쉬 메모리 해제
		delete pObj;            // 객체 메모리 해제
	}
	m_GameObjects.clear(); 
}

void CScene::AnimateObjects(float time) {
	// 벡터(리스트) 순회 중 삭제를 안전하게 하기 위해 iterator(반복자)를 사용합니다.
	for (auto it = m_GameObjects.begin(); it != m_GameObjects.end(); ) {
		CGameObject* pObj = *it;
		pObj->Animate(time); 

		if (pObj->isdead) {
			// 1. 동적할당된 메모리 직접 해제 (객체가 가진 메쉬와 자기자신)
			if (pObj->GetMesh()) {
				delete pObj->GetMesh();
			}
			delete pObj;

			// 2. 리스트(벡터)에서 완전히 삭제하고, 당겨진 다음 요소의 위치를 반환받음
			it = m_GameObjects.erase(it); 
		}
		else {
			// 3. 지우지 않았을 때만 커서를 다음칸으로 수동 이동
			++it; 
		}
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

	for (auto pObj : m_GameObjects) {		
		pObj->SetWorldMatrix();
		mpipeline.SetWorldMatrix(pObj->GetWorldMatrix());
		pObj->Draw(hDC, mpipeline); 
	}
}