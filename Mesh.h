#pragma once
#include "PCH.h"
#include "PipeLine.h"

class CVertex {
public:
	CVertex() {}
	CVertex(float x, float y, float z) {
		v.x = x; v.y = y; v.z = z;
	}
	virtual ~CVertex() {}

	XMFLOAT3 v;
};

class CFace {
public:
	CFace() {}
	CFace(CVertex v1, CVertex v2, CVertex v3) {
		Indices[0] = v1; Indices[1] = v2; Indices[2] = v3;
	}
	virtual ~CFace() {}
	
	CVertex Indices[3];
};

class CMesh {
public:
	CMesh() {}
	virtual ~CMesh() {}

	void Draw(HDC hDC, CPipeLine& pipeline);

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