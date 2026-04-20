#include "Mesh.h"

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

	CalculateLocalBoundingBox();
}

CPlaneMesh::CPlaneMesh(float w, float h) {
	float fHalfWidth = w * 0.5f;
	float fHalfHeight = h * 0.5f;
	VerticesArray = {
		CVertex(-fHalfWidth, 0, +fHalfHeight), // 0
		CVertex(+fHalfWidth, 0, +fHalfHeight), // 1
		CVertex(+fHalfWidth, 0, -fHalfHeight), // 2
		CVertex(-fHalfWidth, 0, -fHalfHeight)  // 3
	};
	IndicesArray = {
		0, 1, 2,
		0, 2, 3
	};
	CalculateLocalBoundingBox();
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

	CalculateLocalBoundingBox();
}