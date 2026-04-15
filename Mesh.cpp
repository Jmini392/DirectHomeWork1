#include "Mesh.h"

void DrawFace(HDC hDC, CVertex v1, CVertex v2, CVertex v3, COLORREF color) {
	POINT points[3] = {
		{ (LONG)v1.v.x, (LONG)v1.v.y },
		{ (LONG)v2.v.x, (LONG)v2.v.y },
		{ (LONG)v3.v.x, (LONG)v3.v.y }
	};

	// 삼각형 그리기
	::Polygon(hDC, points, 3);
}

// --- 새로운 래스터라이저 (Barycentric Coordinates 방식을 덧셈 연산으로 최적화) ---
void DrawFaceZBuffer(HDC hDC, CVertex v1, CVertex v2, CVertex v3, CPipeLine& pipeline, COLORREF color) {
	int width = pipeline.GetWidth();
	int height = pipeline.GetHeight();
	auto& zBuffer = pipeline.GetZBuffer();
	DWORD* pixelBuffer = pipeline.GetPixelBuffer(); // 얻어오기!

	// 1. 삼각형을 포함하는 최소한의 사각형(Bounding Box)을 구합니다.
	int minX = (std::max)(0, (int)(std::min)({ v1.v.x, v2.v.x, v3.v.x }));
	int maxX = (std::min)(width - 1, (int)(std::max)({ v1.v.x, v2.v.x, v3.v.x }));
	int minY = (std::max)(0, (int)(std::min)({ v1.v.y, v2.v.y, v3.v.y }));
	int maxY = (std::min)(height - 1, (int)(std::max)({ v1.v.y, v2.v.y, v3.v.y }));

	// 삼각형의 전체 넓이 계산
	float area = (v2.v.x - v1.v.x) * (v3.v.y - v1.v.y) - (v3.v.x - v1.v.x) * (v2.v.y - v1.v.y);
	if (fabs(area) < 0.0001f) return; // 선분 형태의 삼각형은 무시
	
	// 나눗셈을 한 번만 수행하기 위해 역수를 구함
	float invArea = 1.0f / area;

	// [최적화] X, Y 증가에 따른 w0, w1, w2의 변화량(Step 값)을 미리 계산
	float dw0_dx = (v2.v.y - v3.v.y) * invArea;
	float dw0_dy = (v3.v.x - v2.v.x) * invArea;
	float dw1_dx = (v3.v.y - v1.v.y) * invArea;
	float dw1_dy = (v1.v.x - v3.v.x) * invArea;
	float dw2_dx = (v1.v.y - v2.v.y) * invArea;
	float dw2_dy = (v2.v.x - v1.v.x) * invArea;

	// 시작 픽셀에서의 초기 가중치 계산
	float px_start = (float)minX + 0.5f;
	float py_start = (float)minY + 0.5f;
	
	float w0_row = ((v2.v.x - px_start) * (v3.v.y - py_start) - (v3.v.x - px_start) * (v2.v.y - py_start)) * invArea;
	float w1_row = ((v3.v.x - px_start) * (v1.v.y - py_start) - (v1.v.x - px_start) * (v3.v.y - py_start)) * invArea;
	float w2_row = 1.0f - w0_row - w1_row;

	// [최적화] COLORREF 포맷을 여기서 단 한 번만 DWORD 형태로 변환하여 화면에 그릴 색상을 캐싱합니다.
	DWORD finalColor = 0;
	if (pixelBuffer != nullptr) {
		BYTE r = GetRValue(color);
		BYTE g = GetGValue(color);
		BYTE b = GetBValue(color);
		finalColor = (r << 16) | (g << 8) | b;
	}

	// 2. Bounding Box 순회
	for (int y = minY; y <= maxY; ++y) {
		float w0 = w0_row, w1 = w1_row, w2 = w2_row;
		int rowOffset = y * width; // y축 오프셋을 루프 밖에서 1회 계산

		for (int x = minX; x <= maxX; ++x) {
			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				float currentZ = (w0 * v1.v.z) + (w1 * v2.v.z) + (w2 * v3.v.z);
				int pixelIndex = rowOffset + x;

				if (currentZ < zBuffer[pixelIndex]) {
					zBuffer[pixelIndex] = currentZ; 

					if (pixelBuffer != nullptr) {
						pixelBuffer[pixelIndex] = finalColor;
					}
				}
			}
			w0 += dw0_dx; w1 += dw1_dx; w2 += dw2_dx;
		}
		w0_row += dw0_dy; w1_row += dw1_dy; w2_row += dw2_dy;
	}
}

void CMesh::Draw(HDC hDC, CPipeLine& pipeline, COLORREF color) {
	for (int i = 0; i < IndicesArray.size(); i += 3) {
		CFace face = CFace(
			VerticesArray[IndicesArray[i]],
			VerticesArray[IndicesArray[i + 1]],
			VerticesArray[IndicesArray[i + 2]]
		);

		// 월드 & 뷰 변환
		face.Vertex[0].v = pipeline.WorldViewTransform(face.Vertex[0].v);
		face.Vertex[1].v = pipeline.WorldViewTransform(face.Vertex[1].v);
		face.Vertex[2].v = pipeline.WorldViewTransform(face.Vertex[2].v);

		// 카메라 Z면 컬링 (Near Plane Culling)
		// 점 세 개 중 하나라도 카메라의 눈(0.0f 부근) 뒤로 넘어간다면 그리기를 포기
		if (face.Vertex[0].v.z <= 0.01f || face.Vertex[1].v.z <= 0.01f || face.Vertex[2].v.z <= 0.01f) continue;

		// 백페이스 컬링
		XMFLOAT3 edge1 = Vector3::Subtract(face.Vertex[1].v, face.Vertex[0].v);
		XMFLOAT3 edge2 = Vector3::Subtract(face.Vertex[2].v, face.Vertex[0].v);
		XMFLOAT3 faceNormal = Vector3::CrossProduct(edge1, edge2);
		faceNormal = Vector3::Normalize(faceNormal);
		face.Normal = faceNormal;
		if (Vector3::DotProduct(face.Normal, face.Vertex[0].v) >= 0) continue;

		// 투영 및 뷰포트 변환
		face.Vertex[0].v = pipeline.ProjViewPortTransform(face.Vertex[0].v);
		face.Vertex[1].v = pipeline.ProjViewPortTransform(face.Vertex[1].v);
		face.Vertex[2].v = pipeline.ProjViewPortTransform(face.Vertex[2].v);

		// 그리기
		DrawFaceZBuffer(hDC, face.Vertex[0], face.Vertex[1], face.Vertex[2], pipeline, color);		
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
		CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth), // 4
		CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth), // 5
		CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth), // 6
		CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth)  // 7
	};

	IndicesArray = {
		0, 1, 2, 0, 2, 3, // Front face
		4, 5, 6, 4, 6, 7, // Back face
		0, 5, 4, 0, 4, 1, // Top face
		2, 6, 3, 2, 7, 6, // Bottom face
		3, 5, 0, 3, 6, 5, // Left face
		1, 7, 2, 1, 4, 7  // Right face
	};
}

CObjMesh::CObjMesh(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		// 파일 열기 실패 시 처리 (필요에 따라 에러 로깅)
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string prefix;
		ss >> prefix;

		if (prefix == "v") {
			// 정점(Vertex) 데이터: v x y z
			float x, y, z;
			ss >> x >> y >> z;
			VerticesArray.push_back(CVertex(x, y, z));
		}
		else if (prefix == "f") {
			// 면(Face) 데이터: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
			// 이 예제에서는 다각형이 삼각형(3개의 점)으로 구성되어 있다고 가정합니다.
			for (int i = 0; i < 3; ++i) {
				std::string faceData;
				ss >> faceData;

				// 슬래시('/') 기호를 기준으로 문자열을 잘라내어 정점 인덱스만 추출
				size_t slashPos = faceData.find('/');
				std::string vIdxStr = (slashPos != std::string::npos) ? faceData.substr(0, slashPos) : faceData;

				// .obj 파일의 인덱스는 1부터 시작하므로 0부터 시작하도록 1을 빼줍니다.
				int vIdx = std::stoi(vIdxStr) - 1;
				IndicesArray.push_back(vIdx);
			}
		}
	}
}