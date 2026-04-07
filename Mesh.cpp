#include "PCH.h"
#include "Mesh.h"

CVertex WorldTransform(CVertex vertex) {
	CVertex f3Result;
	// 월드 변환 행렬을 생성한다. (회전, 이동, 크기 변환)
	// 예시로 기준(항등) 위치를 반환하거나 기본적인 크기 변환 후 진행합니다.
	XMMATRIX mWorld = XMMatrixIdentity(); // 필요 시 XMMatrixScaling, Rotation, Translation 조합
	
	XMVECTOR v = XMLoadFloat3(&vertex.v);
	v = XMVector3TransformCoord(v, mWorld);
	XMStoreFloat3(&f3Result.v, v);
	
	return f3Result;
}

CVertex CameraTransform(CVertex vertex, CCamera camera) {
	CVertex f3Result;
	// 카메라 변환 행렬을 생성한다. (카메라의 위치와 방향에 따라)
	// 카메라의 월드 행렬의 역행렬(Inverse)이 곧 뷰 행렬(View Matrix)입니다.
	XMMATRIX mCamera = XMLoadFloat4x4(&camera.cameraMatrix);
	XMVECTOR det = XMMatrixDeterminant(mCamera);
	XMMATRIX mView = XMMatrixInverse(&det, mCamera);

	XMVECTOR v = XMLoadFloat3(&vertex.v);
	v = XMVector3TransformCoord(v, mView);
	XMStoreFloat3(&f3Result.v, v);

	return f3Result;
}

CVertex ProjectionTransform(CVertex vertex) {
	CVertex f3Result;
	// 원근 투영 변환 행렬을 생성한다. (카메라의 시야각 45도, 종횡비, 근평면 1.0, 원평면 1000.0)
	// FRAME_BUFFER_WIDTH, HEIGHT 상수가 PCH.h 등에 정의되어 있다고 가정합니다.
	float fAspect = (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT;
	XMMATRIX mProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), fAspect, 1.0f, 1000.0f);

	XMVECTOR v = XMLoadFloat3(&vertex.v);
	v = XMVector3TransformCoord(v, mProj);
	XMStoreFloat3(&f3Result.v, v);
	
	return f3Result;
}

CVertex ViewportTransform(CVertex vertex) {
	CVertex f3Result;
	// 뷰포트 변환 행렬을 생성한다. (화면의 크기와 위치에 따라)
	// 뷰포트 변환 행렬을 모든 다각형에 적용한다.
	vertex.v.x = (vertex.v.x + 1.f) * 0.5f * FRAME_BUFFER_WIDTH;
	vertex.v.y = (1.f - vertex.v.y) * 0.5f * FRAME_BUFFER_HEIGHT;
	f3Result = vertex;

	return f3Result;
}

void DrawFace(HDC hDC, CVertex v1, CVertex v2, CVertex v3) {
	POINT points[3] = {
		{ (LONG)v1.v.x, (LONG)v1.v.y },
		{ (LONG)v2.v.x, (LONG)v2.v.y },
		{ (LONG)v3.v.x, (LONG)v3.v.y }
	};

	// 삼각형 그리기
	::Polygon(hDC, points, 3);
}

CMesh::~CMesh() {
	delete[] VerticesArray;
	delete[] IndicesArray;
}

void CMesh::Draw(HDC hDC, CCamera* camera) {
	CVertex f3V1, f3V2, f3V3;
	// 삼각형 단위로 처리하므로 +3씩 증가
	int numIndices = IndicesNum;

	// 브러시와 펜
	HBRUSH hOldBrush = (HBRUSH)::GetCurrentObject(hDC, OBJ_BRUSH);
	HPEN hOldPen = (HPEN)::GetCurrentObject(hDC, OBJ_PEN);
	
	// 메시의 색상으로 브러시 생성
	HBRUSH hBrush = ::CreateSolidBrush(MeshColor);
	::SelectObject(hDC, hBrush);
	::SelectObject(hDC, ::GetStockObject(NULL_PEN));

	for (int i = 0; i < numIndices; i += 3) {
		// 인덱스를 참조하여 실제 버텍스 데이터를 가져옴
		f3V1 = VerticesArray[IndicesArray[i]];
		f3V2 = VerticesArray[IndicesArray[i + 1]];
		f3V3 = VerticesArray[IndicesArray[i + 2]];

		// 1. 월드 변환
		f3V1 = WorldTransform(f3V1);
		f3V2 = WorldTransform(f3V2);
		f3V3 = WorldTransform(f3V3);

		// 2. 카메라 변환
		if (camera) { 
			f3V1 = CameraTransform(f3V1, *camera);
			f3V2 = CameraTransform(f3V2, *camera);
			f3V3 = CameraTransform(f3V3, *camera);
		}

		// 3. 투영 변환
		f3V1 = ProjectionTransform(f3V1);
		f3V2 = ProjectionTransform(f3V2);
		f3V3 = ProjectionTransform(f3V3);

		// 4. 뷰포트 변환
		f3V1 = ViewportTransform(f3V1);
		f3V2 = ViewportTransform(f3V2);
		f3V3 = ViewportTransform(f3V3);

		// 화면 내에 점이 포함되는지 테스트
		if ((f3V1.v.x > -100.f && f3V2.v.x > -100.f && f3V3.v.x > -100.f) &&
			(f3V1.v.y > -100.f && f3V2.v.y > -100.f && f3V3.v.y > -100.f)) {
			::DrawFace(hDC, f3V1, f3V2, f3V3);
		}
	}

	// 루프 종료 후 브러시 및 펜 메모리 복구 / 해제
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);  // PEN 돌려놓기
	::DeleteObject(hBrush);
}

CCubeMesh::CCubeMesh(float w, float h, float d) {
	float fHalfWidth = w * 0.5f;
	float fHalfHeight = h * 0.5f;
	float fHalfDepth = d * 0.5f;

	VerticesArray = new CVertex[8] {
		CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth), // 0
		CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth), // 1
		CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth), // 2
		CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth), // 3
		CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth), // 4
		CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth), // 5
		CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth), // 6
		CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth)  // 7
	};
	
	IndicesArray = new DWORD[36] {
		0, 1, 2, 0, 2, 3, // Front face
		4, 5, 6, 4, 6, 7, // Back face
		0, 1, 5, 0, 5, 4, // Top face
		3, 2, 6, 3, 6, 7, // Bottom face
		0, 3, 7, 0, 7, 4, // Left face
		1, 2, 6, 1, 6, 5  // Right face
	};

	IndicesNum = 36;
}