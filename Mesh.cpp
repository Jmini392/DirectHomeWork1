#include "PCH.h"
#include "PipeLine.h"
#include "Mesh.h"

void DrawFace(HDC hDC, CVertex v1, CVertex v2, CVertex v3) {
	POINT points[3] = {
		{ (LONG)v1.v.x, (LONG)v1.v.y },
		{ (LONG)v2.v.x, (LONG)v2.v.y },
		{ (LONG)v3.v.x, (LONG)v3.v.y }
	};

	// 삼각형 그리기
	::Polygon(hDC, points, 3);
}

void CMesh::Draw(HDC hDC, CPipeLine& pipeline) {
	CVertex f3V1, f3V2, f3V3;
	// 삼각형 단위로 처리하므로 +3씩 증가
	int numIndices = IndicesNum;
	
	for (int i = 0; i < numIndices; i += 3) {
		// 인덱스를 참조하여 실제 버텍스 데이터를 가져옴
		f3V1 = VerticesArray[IndicesArray[i]];
		f3V2 = VerticesArray[IndicesArray[i + 1]];
		f3V3 = VerticesArray[IndicesArray[i + 2]];

		f3V1.v = pipeline.MatrixTransform(f3V1.v);
		f3V2.v = pipeline.MatrixTransform(f3V2.v);
		f3V3.v = pipeline.MatrixTransform(f3V3.v);

		// 화면 내에 점이 포함되는지 테스트
		if ((f3V1.v.x > -100.f && f3V2.v.x > -100.f && f3V3.v.x > -100.f) &&
			(f3V1.v.y > -100.f && f3V2.v.y > -100.f && f3V3.v.y > -100.f)) {
			::DrawFace(hDC, f3V1, f3V2, f3V3);
		}
	}
}

CCubeMesh::CCubeMesh(float w, float h, float d) {
	float fHalfWidth = w * 0.5f;
	float fHalfHeight = h * 0.5f;
	float fHalfDepth = d * 0.5f;

	VerticesArray = {
		CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth), // 0
		CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth), // 1
		CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth), // 2
		CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth), // 3
		CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth), // 4
		CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth), // 5
		CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth), // 6
		CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth)  // 7
	};

	IndicesArray = {
		0, 1, 2, 0, 2, 3, // Front face
		4, 5, 6, 4, 6, 7, // Back face
		0, 1, 5, 0, 5, 4, // Top face
		3, 2, 6, 3, 6, 7, // Bottom face
		0, 3, 7, 0, 7, 4, // Left face
		1, 2, 6, 1, 6, 5  // Right face
	};

	IndicesNum = IndicesArray.size();
}