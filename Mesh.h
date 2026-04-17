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