#pragma once
#include "PCH.h"
#include "PipeLine.h"
#include "Mesh.h"

class CGameObject {
public:
	CGameObject() {}
	virtual ~CGameObject() {}
	
	void SetMesh(std::shared_ptr<CMesh> pMesh) { mesh = pMesh; }
	void SetPosition(float x, float y, float z) { Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }
	void SetColor(COLORREF color) { MeshColor = color; }
	void SetWorldMatrix();

	std::shared_ptr<CMesh> GetMesh() { return mesh; }
	XMFLOAT3 GetPosition() { return Position; }
	XMFLOAT3 GetRotation() { return Rotation; }
	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }

	void Draw(HDC hDC, CPipeLine& pipeline);
	virtual void Animate(float time);

	void Move(float x, float y, float z) { Position.x += x; Position.y += y; Position.z += z; }
	void Rotate(float x, float y, float z) { Rotation.x += x; Rotation.y += y; Rotation.z += z; }

	bool isdead = false;
private:
	std::shared_ptr<CMesh> mesh; // 메시 객체
	XMFLOAT3 Position = { 0.f, 0.f, 0.f }; // 게임 오브젝트의 위치
	XMFLOAT3 Rotation = { 0.f, 0.f, 0.f }; // 게임 오브젝트의 회전
	COLORREF MeshColor = RGB(255, 0, 0); // 게임 오브젝트의 색상
	XMFLOAT4X4 WorldMatrix = Matrix4x4::Identity(); // 월드 행렬
};

class CRotate : public CGameObject {
public:
	CRotate() {}
	virtual ~CRotate() {}

	virtual void Animate(float time);
};

class CBullet : public CGameObject {
public:
    CBullet() {}
	virtual ~CBullet() {}

    void SetDirection(XMFLOAT3 dir) { Direction = dir; }
    void SetSpeed(float s) { Speed = s; }
	void SetStartPosition(float x, float y, float z) { StartPosition = XMFLOAT3(x, y, z); }

 	virtual void Animate(float time);
private:
	XMFLOAT3 Direction = {0.f, 0.f, 1.f}; // 날아갈 방향
	float Speed = 1.f; // 날아갈 속도
	XMFLOAT3 StartPosition = { 0.f, 0.f, 0.f }; // 총알이 발사된 위치
};