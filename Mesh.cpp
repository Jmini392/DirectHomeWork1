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
	// 리스트 준비
	std::vector<CFace> faceList;

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

		// 백페이스 컬링
		XMFLOAT3 edge1 = Vector3::Subtract(face.Vertex[1].v, face.Vertex[0].v);
		XMFLOAT3 edge2 = Vector3::Subtract(face.Vertex[2].v, face.Vertex[0].v);
		XMFLOAT3 faceNormal = Vector3::CrossProduct(edge1, edge2);
		faceNormal = Vector3::Normalize(faceNormal);
		face.Normal = faceNormal;

		if (!pipeline.CameraDot(face.Normal, face.Vertex[0].v)) {
			continue;
		}

		// 투영 및 뷰포트 변환
		face.Vertex[0].v = pipeline.ProjViewPortTransform(face.Vertex[0].v);
		face.Vertex[1].v = pipeline.ProjViewPortTransform(face.Vertex[1].v);
		face.Vertex[2].v = pipeline.ProjViewPortTransform(face.Vertex[2].v);

		faceList.push_back(face);
	}

	// 모든 면을 깊이 값 기준으로 한 번만 정렬
	std::sort(faceList.begin(), faceList.end(), [](const CFace& a, const CFace& b) {
		float depthA = (a.Vertex[0].v.z + a.Vertex[1].v.z + a.Vertex[2].v.z) / 3.0f;
		float depthB = (b.Vertex[0].v.z + b.Vertex[1].v.z + b.Vertex[2].v.z) / 3.0f;
		return depthA > depthB; // 내림차순: 먼 것부터 그리기
		});

	// 그리기
	for (const auto& face : faceList) {
		DrawFace(hDC, face.Vertex[0], face.Vertex[1], face.Vertex[2]);
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