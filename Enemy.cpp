#include "Enemy.h"

CEnemy::CEnemy() {
	// 메시 크기 객체 고정
	std::shared_ptr<CMesh> pEnemyMesh = std::make_shared<CCubeMesh>(6.f, 6.f, 6.f);
	// 적 객체 초기화
	SetMesh(pEnemyMesh);
}