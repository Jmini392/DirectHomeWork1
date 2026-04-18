#pragma once
#include "PCH.h"
#include "Mesh.h"

class CGameObject {
public:
	CGameObject() {}
	virtual ~CGameObject() {}
	
	void SetMesh(std::shared_ptr<CMesh> pMesh) { mesh = pMesh; }
	virtual void SetPosition(float x, float y, float z) { Position = XMFLOAT3(x, y, z); }
	void SetRotation(float x, float y, float z) { Rotation = XMFLOAT3(x, y, z); }
	void SetColor(COLORREF color) { MeshColor = color; }
	void SetWorldMatrix();

	std::shared_ptr<CMesh> GetMesh() { return mesh; }
	XMFLOAT3 GetPosition() { return Position; }
	XMFLOAT3 GetRotation() { return Rotation; }
	COLORREF GetColor() { return MeshColor; }
	XMFLOAT4X4 GetWorldMatrix() { return WorldMatrix; }

	virtual void Animate(float time) {}

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

class CWall : public CGameObject {
public:
	CWall();
	virtual ~CWall() {}
};

class CFloor : public CGameObject {
public:
	CFloor();
	virtual ~CFloor() {}
};

class CItem : public CGameObject {
public:
	CItem();
	virtual ~CItem() {}

	virtual void Animate(float time);

	float randomValue;
};

class CBullet : public CGameObject {
public:
    CBullet() {}
	virtual ~CBullet() {}

	virtual void SetPosition(float x, float y, float z) override { 
		CGameObject::SetPosition(x, y, z);
		StartPosition = XMFLOAT3(x, y, z); // 총알이 발사된 위치 저장
	}
    void SetDirection(XMFLOAT3 dir) { Direction = dir; }
    void SetSpeed(float s) { Speed = s; }
	
 	virtual void Animate(float time);
private:
	XMFLOAT3 Direction = {0.f, 0.f, 1.f}; // 날아갈 방향
	float Speed = 1.f; // 날아갈 속도
	XMFLOAT3 StartPosition = { 0.f, 0.f, 0.f }; // 총알이 발사된 위치
};