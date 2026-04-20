#pragma once
#include "PCH.h"

class CVertex {
public:
	CVertex() {}
	CVertex(float x, float y, float z) { v.x = x; v.y = y; v.z = z; }
	virtual ~CVertex() {}

	XMFLOAT3 v = { 0.0f, 0.0f, 0.0f };
};

class CFace {
public:
	CFace() {}
	CFace(CVertex v1, CVertex v2, CVertex v3) { Vertex[0] = v1; Vertex[1] = v2; Vertex[2] = v3; }
	virtual ~CFace() {}

	// 정점, 법선벡터
	CVertex Vertex[3];
	XMFLOAT3 Normal = { 0.0f, 0.0f, 0.0f };
};

class CMesh {
public:
	CMesh() {}
	virtual ~CMesh() {}

	std::vector<CVertex> VerticesArray; // 정점 배열
	std::vector<DWORD> IndicesArray; // 인덱스 배열

	// DirectX 바운딩 박스 객체
	BoundingOrientedBox m_LocalBoundingBox;

	// 메시 버텍스 세팅 이후 이 함수를 한 번 호출해 줍니다. (CubeMesh나 ObjMesh 생성자의 마지막)
	void CalculateLocalBoundingBox() {
		if (VerticesArray.empty()) return;

		// CreateFromPoints 인자: 바운딩 박스 객체, 버텍스 개수, 첫 번째 XMFLOAT3 포인터, 구조체 크기 간격
		BoundingOrientedBox::CreateFromPoints(
			m_LocalBoundingBox,
			VerticesArray.size(),
			&(VerticesArray[0].v),
			sizeof(CVertex)
		);
	}
};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(float w = 4.f, float h = 4.f, float d = 4.f);
	virtual ~CCubeMesh() {}
};

class CObjMesh : public CMesh {
public:
	CObjMesh(const std::string& filename);
	virtual ~CObjMesh() {}
};