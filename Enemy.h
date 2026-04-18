#pragma once
#include "PCH.h"
#include "GameObject.h"

class CEnemy : public CGameObject {
public:
	CEnemy();
	virtual ~CEnemy() {}
	virtual void Animate(float time) {}
};