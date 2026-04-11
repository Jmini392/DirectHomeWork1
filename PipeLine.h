#pragma once
#include "PCH.h"

class CPipeLine {
public:
	CPipeLine() {}
	virtual ~CPipeLine() {}

	void SetWorldMatrix(const XMFLOAT4X4& world) { mWorld = world; }
	void SetViewMatrix(const XMFLOAT4X4& view) { mView = view; }
	void SetProjMatrix(const XMFLOAT4X4& proj) { mProj = proj; }
	void SetViewportMatrix(const XMFLOAT4X4& viewport) { mViewport = viewport; }
	
	bool CameraDot(XMFLOAT3& faceNormal, XMFLOAT3& vertexInView);

	XMFLOAT3 WorldViewTransform(XMFLOAT3& point);
	XMFLOAT3 ProjViewPortTransform(XMFLOAT3& point);
private:
	XMFLOAT4X4 mWorld; // 월드 변환 행렬
	XMFLOAT4X4 mView; // 뷰 변환 행렬
	XMFLOAT4X4 mProj; // 원근 투영 변환 행렬
	XMFLOAT4X4 mViewport; // 화면 좌표 변환 행렬
};