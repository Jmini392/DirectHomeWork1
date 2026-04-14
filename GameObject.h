#pragma once
#include "PCH.h"
#include "PipeLine.h"
#include "Mesh.h"

class CGameObject {
public:
	CGameObject() {}
	virtual ~CGameObject() {}
	
	void SetPosition(float x, float y, float z) { Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }
	XMFLOAT3 GetPosition() { return Position; }

	void Move(float x, float y, float z) { Position.x += x; Position.y += y; Position.z += z; }
	void Rotate(float x, float y, float z) { Rotation.x += x; Rotation.y += y; Rotation.z += z; }

	void SetColor(COLORREF color) { MeshColor = color; }
	void SetMesh(CMesh* mesh) { this->mesh = mesh; }
	CMesh* GetMesh() { return mesh; }

	void SetWorldMatrix();
	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }

	void Draw(HDC hDC, CPipeLine& pipeline);
	virtual void Animate(float time);

	bool isdead = false;
private:
	CMesh* mesh = nullptr; // 메시 객체
	COLORREF MeshColor = RGB(255, 0, 0); // 게임 오브젝트의 색상

	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity(); // 월드 행렬
	
	XMFLOAT3 Position = {0.f, 0.f, 0.f}; // 게임 오브젝트의 위치
	XMFLOAT3 Rotation = {0.f, 0.f, 0.f}; // 게임 오브젝트의 회전
};

class CBullet : public CGameObject {
public:
    CBullet() {}
	virtual ~CBullet() {}

    void SetDirection(XMFLOAT3 dir) { Direction = dir; }
    void SetSpeed(float s) { Speed = s; }
	void SetStartPosition(float x, float y, float z) { StartPosition = XMFLOAT3(x, y, z); }

    // 매 프레임마다 방향 * 속도만큼 이동
	virtual void Animate(float time) override;
private:
	XMFLOAT3 Direction = {0.f, 0.f, 1.f}; // 날아갈 방향
	float Speed = 1.f; // 날아갈 속도
	XMFLOAT3 StartPosition = { 0.f, 0.f, 0.f }; // 총알이 발사된 위치
};

class CWall : public CGameObject {
public:
	CWall() {}
	virtual ~CWall() {}
	// 벽은 움직이지 않게
	virtual void Animate(float time) override {}
private:	
};