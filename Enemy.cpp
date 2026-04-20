#include "Enemy.h"

CEnemy::CEnemy(int num) {
	SetType(ObjectType::ENEMY);
	std::shared_ptr<CMesh> pEnemyMesh;
	// 메시 크기 객체 고정
	if (num == 0) {
		pEnemyMesh = std::make_shared<CCubeMesh>(4.f, 4.f, 4.f);
		SetColor(RGB(255, 0, 0));
		m_Speed = 7.0f;
		health = 1;
	}
	else if (num == 1) {
		pEnemyMesh = std::make_shared<CCubeMesh>(5.f, 5.f, 5.f);
		SetColor(RGB(0, 255, 0));
		m_Speed = 5.0f;
		health = 2;
	}
	else if (num == 2) {
		pEnemyMesh = std::make_shared<CCubeMesh>(6.f, 6.f, 6.f);
		SetColor(RGB(0, 0, 255));
		m_Speed = 3.0f;
		health = 3;
	}
	// 적 객체 초기화
	SetMesh(pEnemyMesh);
}

void CEnemy::Animate(float time) {
	// 타겟이 살아있고 유효한지 확인
	if (auto pTarget = m_pTarget.lock()) { 
		// 적과 타겟(플레이어)의 위치 가져오기
		XMFLOAT3 currentPos = GetPosition();
		XMFLOAT3 targetPos = pTarget->GetPosition();

		// 방향 벡터 계산 (도착지 - 출발지)
		XMVECTOR vCurrent = XMLoadFloat3(&currentPos);
		XMVECTOR vTarget = XMLoadFloat3(&targetPos);
		XMVECTOR vDir = XMVectorSubtract(vTarget, vCurrent);
		
		// 거리가 너무 가까우면 이동 정지 (떨림 현상 방지)
		float distance = XMVectorGetX(XMVector3Length(vDir));
		if (distance > 0.5f) {
			// 방향 벡터를 정규화(길이 1)
			vDir = XMVector3Normalize(vDir);

			// 속도 적용 (time(DeltaTime)을 곱하여 프레임에 독립적이도록 처리하는 것이 좋음, 필요시 time 사용)
			float moveDist = m_Speed * time;
			vDir = XMVectorScale(vDir, moveDist);
			// 이동량 추출
			XMFLOAT3 moveDelta;
			XMStoreFloat3(&moveDelta, vDir);

			// 기존 위치에 더하기
			Move(moveDelta.x, 0.f, moveDelta.z);
		}
	}
}

void CEnemy::OnCollision(std::shared_ptr<CGameObject> pOther) {
	// 총알과 충돌 시 소멸 처리
	if (pOther->GetType() == ObjectType::BULLET) {
		if (--health <= 0) isdead = true;
	}
}