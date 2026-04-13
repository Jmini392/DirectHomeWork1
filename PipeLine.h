#pragma once
#include "PCH.h"

class CPipeLine {
public:
	CPipeLine() {}
	virtual ~CPipeLine() {}

	// --- 새로 추가되는 Z-Buffer 관련 함수들 ---
	void InitZBuffer(int width, int height);
	void ClearZBuffer();
	std::vector<float>& GetZBuffer() { return m_ZBuffer; }
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	void SetWorldMatrix(const XMFLOAT4X4& world) { mWorld = world; }
	void SetViewMatrix(const XMFLOAT4X4& view) { mView = view; }
	void SetProjMatrix(const XMFLOAT4X4& proj) { mProj = proj; }
	void SetViewportMatrix(const XMFLOAT4X4& viewport) { mViewport = viewport; }
	
	bool CameraDot(XMFLOAT3& faceNormal, XMFLOAT3& vertexInView);

	XMFLOAT3 WorldViewTransform(XMFLOAT3& point);
	XMFLOAT3 ProjViewPortTransform(XMFLOAT3& point);

	void SetPixelBuffer(DWORD* pBuffer) { m_pPixelBuffer = pBuffer; }
	DWORD* GetPixelBuffer() { return m_pPixelBuffer; }
private:
	XMFLOAT4X4 mWorld; // 월드 변환 행렬
	XMFLOAT4X4 mView; // 뷰 변환 행렬
	XMFLOAT4X4 mProj; // 원근 투영 변환 행렬
	XMFLOAT4X4 mViewport; // 화면 좌표 변환 행렬

	// Z-Buffer 변수 추가
	std::vector<float> m_ZBuffer;
	int m_Width = 0;
	int m_Height = 0;

	DWORD* m_pPixelBuffer = nullptr;
};