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

	void SetCameraLookVector(const XMFLOAT3& lookVector) { m_CameraLookVector = lookVector; }
	
	bool CameraDot(XMFLOAT3& vector);

	XMFLOAT3 MatrixTransform(XMFLOAT3& point);
private:
	XMFLOAT4X4 mWorld; // 월드 변환 행렬
	XMFLOAT4X4 mView; // 뷰 변환 행렬
	XMFLOAT4X4 mProj; // 원근 투영 변환 행렬
	XMFLOAT4X4 mViewport; // 화면 좌표 변환 행렬

	// 카메라의 시선벡터 저장
	XMFLOAT3 m_CameraLookVector;
};