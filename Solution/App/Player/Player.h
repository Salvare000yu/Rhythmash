#pragma once
#include "BaseActObj/BaseActObj.h"
#include "Input/Input.h"
#include <memory>
#include <DirectXMath.h>
class Player
	: public BaseActObj
{
	Input* input = nullptr;

	
private:
	DirectX::XMFLOAT3 dir = { 0,0,0 };

	int WaitTime = 0;
public:
	Player(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });
	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	void update();
	void Move()  override;
	void Attack() override;

};

