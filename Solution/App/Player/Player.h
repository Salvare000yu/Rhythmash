#pragma once
#include "BaseActObj/BaseActObj.h"
#include <DirectXMath.h>
#include <functional>

class Player
	: public BaseActObj
{
	std::function<void()> update_proc;

public:
	Player(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });
	inline auto createCollider() { return CollisionMgr::ColliderType{.obj = this, .colliderR = this->getScaleF3().z }; }

	void Move()  override;
	void Attack() override;
	void Step();
};

