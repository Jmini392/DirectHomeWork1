#pragma once
#include "Mesh.h"

class CCamera {
public:
	CCamera();
	virtual ~CCamera() {}

	void SetCamera();
	void SetPosition(float x, float y, float z);
	void SetRotattion(float x, float y, float z);
	void SetVector();

	XMFLOAT4X4 cameraMatrix; // 카메라 저장 행렬
};