#pragma once
#include "BaseActObj/BaseActObj.h"
#include "Input/Input.h"
#include <memory>
#include "3D/Obj/ObjModel.h"
#include "GameObject/GameObj.h"
#include "Camera/CameraObj.h"
#include <DirectXMath.h>
class Player
	: public BaseActObj
{
	Input* input = nullptr;

	const float MOVE_SPEED = GetSpeed();

	DirectX::XMFLOAT3 dir = { 0,0,0 };

	bool isWeekAttack;
	bool isHardAttack;
	bool isJump;
	bool isStep;

public:
	Player(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });
	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	void update();
	void Move()  override;
	void Attack() override;
	void Step();
};

