#pragma once
#include "BaseActObj/BaseActObj.h"
#include <DirectXMath.h>
#include <functional>
#include <Sound/Sound.h>

class Input;

class Player
	: public BaseActObj
{
	Input* input = nullptr;
	std::unique_ptr<Sound> se1;

	std::function<void()> update_proc;
	const float MOVE_SPEED = GetSpeed();

	DirectX::XMFLOAT3 dir = { 0,0,0 };
	bool judgeRet = false;

public:
	Player(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });
	inline auto createCollider() { return CollisionMgr::ColliderType{.obj = this, .colliderR = this->getScaleF3().z }; }

	void Move()  override;
	void Attack() override;
	void Step();
	void setJudge(bool judge) { judgeRet = judge; }
};

