#pragma once
#include "Mesh.h"

class CMesh;
class CPipeLine;

class CGameObject {
public:
	CGameObject() {}
	virtual ~CGameObject() {}
	
	void SetMesh(CMesh* mesh) { this->mesh = mesh; }
	void SetWorldMatrix();
	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }

	void Draw(HDC hDC, CPipeLine& pipeline);
private:
	CMesh* mesh = nullptr; // 메시 객체
	COLORREF MeshColor = RGB(255, 0, 0); // 게임 오브젝트의 색상

	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity(); // 월드 행렬
	
	XMFLOAT3 Position = {0.f, 0.f, 0.f}; // 게임 오브젝트의 위치
	XMFLOAT3 Rotation = {0.f, 0.f, 0.f}; // 게임 오브젝트의 회전
};