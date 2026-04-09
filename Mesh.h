#pragma once

class CPipeLine;

class CVertex {
public:
	CVertex() {}
	CVertex(float x, float y, float z) {
		v.x = x; v.y = y; v.z = z;
	}
	virtual ~CVertex() {}

	XMFLOAT3 v;
};

class CMesh {
public:
	CMesh() {}
	virtual ~CMesh() {}

	void Draw(HDC hDC, CPipeLine& pipeline);

	std::vector<CVertex> VerticesArray; // 정점 배열
	std::vector<DWORD> IndicesArray; // 인덱스 배열
	DWORD IndicesNum; // 인덱스 개수
};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(float w = 4.f, float h = 4.f, float d = 4.f);
	virtual ~CCubeMesh() {}
};