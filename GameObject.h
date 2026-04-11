#pragma once
#include "PCH.h"
#include "PipeLine.h"
#include "Mesh.h"

class CMesh;

class CGameObject {
public:
	CGameObject() {}
	virtual ~CGameObject() {}
	
	void SetPosition(float x, float y, float z) { Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }

	void Move(float x, float y, float z) { Position.x += x; Position.y += y; Position.z += z; }
	void Rotate(float x, float y, float z) { Rotation.x += x; Rotation.y += y; Rotation.z += z; }

	void SetColor(COLORREF color) { MeshColor = color; }
	void SetMesh(CMesh* mesh) { this->mesh = mesh; }
	CMesh* GetMesh() { return mesh; }

	void SetWorldMatrix();
	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }

	void Draw(HDC hDC, CPipeLine& pipeline);
	void Animate();
private:
	CMesh* mesh = nullptr; // 메시 객체
	COLORREF MeshColor = RGB(255, 0, 0); // 게임 오브젝트의 색상

	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity(); // 월드 행렬
	
	XMFLOAT3 Position = {0.f, 0.f, 0.f}; // 게임 오브젝트의 위치
	XMFLOAT3 Rotation = {0.f, 0.f, 0.f}; // 게임 오브젝트의 회전
};