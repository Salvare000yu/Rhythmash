#pragma once
#include "BaseActObj/BaseActObj.h"
#include <DirectXMath.h>
#include <functional>

class Input;

class Player
	: public BaseActObj
{
	Input* input = nullptr;

	std::function<void()> update_proc;
	const float MOVE_SPEED = GetSpeed();

	DirectX::XMFLOAT3 dir = { 0,0,0 };

public:
	Player(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });
	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	void additionalUpdate() override;
	void Move()  override;
	void Attack() override;
	void Step();
};

