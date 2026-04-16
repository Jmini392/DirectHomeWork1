#include "PipeLine.h"

void CPipeLine::InitZBuffer(int width, int height) {
	m_Width = width;
	m_Height = height;
	m_ZBuffer.resize(width * height, 1.0f);
}

void CPipeLine::ClearZBuffer() {
	std::fill(m_ZBuffer.begin(), m_ZBuffer.end(), 1.0f);
}

void CPipeLine::DrawFaceZBuffer(HDC hDC, CVertex v1, CVertex v2, CVertex v3, COLORREF color) {
	// 1. 삼각형을 포함하는 최소한의 사각형(Bounding Box)을 구합니다.
	int minX = (std::max)(0, (int)(std::min)({ v1.v.x, v2.v.x, v3.v.x }));
	int maxX = (std::min)(m_Width - 1, (int)(std::max)({ v1.v.x, v2.v.x, v3.v.x }));
	int minY = (std::max)(0, (int)(std::min)({ v1.v.y, v2.v.y, v3.v.y }));
	int maxY = (std::min)(m_Height - 1, (int)(std::max)({ v1.v.y, v2.v.y, v3.v.y }));

	// 삼각형의 전체 넓이 계산
	float area = (v2.v.x - v1.v.x) * (v3.v.y - v1.v.y) - (v3.v.x - v1.v.x) * (v2.v.y - v1.v.y);
	if (fabs(area) < 0.0001f) return; // 선분 형태의 삼각형은 무시

	// 나눗셈을 한 번만 수행하기 위해 역수를 구함
	float invArea = 1.0f / area;

	// [최적화] X, Y 증가에 따른 w0, w1, w2의 변화량(Step 값)을 미리 계산
	float dw0_dx = (v2.v.y - v3.v.y) * invArea;
	float dw0_dy = (v3.v.x - v2.v.x) * invArea;
	float dw1_dx = (v3.v.y - v1.v.y) * invArea;
	float dw1_dy = (v1.v.x - v3.v.x) * invArea;
	float dw2_dx = (v1.v.y - v2.v.y) * invArea;
	float dw2_dy = (v2.v.x - v1.v.x) * invArea;

	// 시작 픽셀에서의 초기 가중치 계산
	float px_start = (float)minX + 0.5f;
	float py_start = (float)minY + 0.5f;

	float w0_row = ((v2.v.x - px_start) * (v3.v.y - py_start) - (v3.v.x - px_start) * (v2.v.y - py_start)) * invArea;
	float w1_row = ((v3.v.x - px_start) * (v1.v.y - py_start) - (v1.v.x - px_start) * (v3.v.y - py_start)) * invArea;
	float w2_row = 1.0f - w0_row - w1_row;

	// [최적화] COLORREF 포맷을 여기서 단 한 번만 DWORD 형태로 변환하여 화면에 그릴 색상을 캐싱합니다.
	DWORD finalColor = 0;
	if (m_pPixelBuffer != nullptr) {
		BYTE r = GetRValue(color);
		BYTE g = GetGValue(color);
		BYTE b = GetBValue(color);
		finalColor = (r << 16) | (g << 8) | b;
	}

	// 2. Bounding Box 순회
	float invZ1 = 1.0f / v1.v.z;
	float invZ2 = 1.0f / v2.v.z;
	float invZ3 = 1.0f / v3.v.z;

	for (int y = minY; y <= maxY; ++y) {
		float w0 = w0_row, w1 = w1_row, w2 = w2_row;
		int rowOffset = y * m_Width;

		for (int x = minX; x <= maxX; ++x) {
			if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
				// [원근 보정 보간] 1/Z 값을 선형 보간함
				float interpolatedInvZ = (w0 * invZ1) + (w1 * invZ2) + (w2 * invZ3);
				float currentZ = 1.0f / interpolatedInvZ;

				int pixelIndex = rowOffset + x;

				if (currentZ < m_ZBuffer[pixelIndex]) {
					m_ZBuffer[pixelIndex] = currentZ;

					if (m_pPixelBuffer != nullptr) {
						m_pPixelBuffer[pixelIndex] = finalColor;
					}
				}
			}
			w0 += dw0_dx; w1 += dw1_dx; w2 += dw2_dx;
		}
		w0_row += dw0_dy; w1_row += dw1_dy; w2_row += dw2_dy;
	}
}

void CPipeLine::DrawObject(HDC hDC, CMesh* obj, COLORREF color) {
	XMMATRIX worldview = XMLoadFloat4x4(&mWorld) * XMLoadFloat4x4(&mView);
	for (int i = 0; i < obj->IndicesArray.size(); i += 3) {
		CFace face = CFace(
			obj->VerticesArray[obj->IndicesArray[i]],
			obj->VerticesArray[obj->IndicesArray[i + 1]],
			obj->VerticesArray[obj->IndicesArray[i + 2]]
		);
		XMVECTOR v1 = XMLoadFloat3(&face.Vertex[0].v);
		XMVECTOR v2 = XMLoadFloat3(&face.Vertex[1].v);
		XMVECTOR v3 = XMLoadFloat3(&face.Vertex[2].v);

		// 월드 & 뷰 변환
		v1 = XMVector3TransformCoord(v1, worldview);
		v2 = XMVector3TransformCoord(v2, worldview);
		v3 = XMVector3TransformCoord(v3, worldview);

		// 카메라 Z면 컬링 (Near Plane Culling)
		// 점 세 개 중 하나라도 카메라의 눈(0.0f 부근) 뒤로 넘어간다면 그리기를 포기
		if (XMVectorGetZ(v1) <= 0.01f || XMVectorGetZ(v2) <= 0.01f || XMVectorGetZ(v3) <= 0.01f) continue;

		// 백페이스 컬링
		XMVECTOR edge1 = v2 - v1;
		XMVECTOR edge2 = v3 - v1;
		XMVECTOR faceNormalVec = XMVector3Cross(edge1, edge2);
		faceNormalVec = XMVector3Normalize(faceNormalVec);
		XMStoreFloat3(&face.Normal, faceNormalVec);
		if (XMVectorGetX(XMVector3Dot(faceNormalVec, v1)) >= 0) continue;
		
		// 투영 변환
		XMMATRIX Proj = XMLoadFloat4x4(&mProj);
		v1 = XMVector3TransformCoord(v1, Proj);
		v2 = XMVector3TransformCoord(v2, Proj);
		v3 = XMVector3TransformCoord(v3, Proj);

		// 원근 나누기
		float w0 = XMVectorGetW(v1);
		float w1 = XMVectorGetW(v2);
		float w2 = XMVectorGetW(v3);
		if (fabs(w0) > 0.000000001f) v1 = XMVectorDivide(v1, XMVectorSplatW(v1));
		if (fabs(w1) > 0.000000001f) v2 = XMVectorDivide(v2, XMVectorSplatW(v2));
		if (fabs(w2) > 0.000000001f) v3 = XMVectorDivide(v3, XMVectorSplatW(v3));
		
		// 뷰포트 변환
		XMMATRIX viewport = XMLoadFloat4x4(&mViewport);
		v1 = XMVector3TransformCoord(v1, viewport);
		v2 = XMVector3TransformCoord(v2, viewport);
		v3 = XMVector3TransformCoord(v3, viewport);

		XMStoreFloat3(&face.Vertex[0].v, v1);
		XMStoreFloat3(&face.Vertex[1].v, v2);
		XMStoreFloat3(&face.Vertex[2].v, v3);

		// 그리기
		DrawFaceZBuffer(hDC, face.Vertex[0], face.Vertex[1], face.Vertex[2], color);
	}
}