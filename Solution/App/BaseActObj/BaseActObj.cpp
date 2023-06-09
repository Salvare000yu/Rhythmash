﻿#include "BaseActObj.h"
#include <Sound/Sound.h>
#include <3D/Obj/ObjModel.h>
#include <algorithm>

using namespace DirectX;

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos) :
	GameObj(camera, model, pos),
	judge([] { return true; })
{
	atkModel = std::make_unique<ObjModel>("Resources/Attack/", "Attack");

	atkObjPt = otherObj.emplace("AtkObj", std::make_unique<GameObj>(camera, atkModel.get())).first->second;

	auto atkObj = atkObjPt.lock();

	atkObj->setParent((BaseObj*)obj.get());
	atkObj->setPos(XMFLOAT3(0, 0, 5));
	atkObj->setCol(XMFLOAT4(1, 1, 1, 0.5f));

	atkcoll = CollisionMgr::ColliderType::create(atkObj.get(), atkObj->getScaleF3().z);
	mycoll = CollisionMgr::ColliderType::create(this, this->getScaleF3().z);
}

void BaseActObj::invincible()
{
	if (!invincibleFrag) { return; }
	if (++invincibleFrame > invincibleFrameMax)
	{
		invincibleFrame = 0;
		invincibleFrag = false;
	}
}

void BaseActObj::moveProcess(const XMFLOAT3& dir)
{
	moveProcess(XMLoadFloat3(&dir));
}

void BaseActObj::moveProcess(const DirectX::XMVECTOR& dir)
{
	const auto moveValVec = XMVector3Normalize(dir) * (moveSpeed / DX12Base::ins()->getFPS());

	XMFLOAT3 moveVal{};
	XMStoreFloat3(&moveVal, moveValVec);
	obj->position.x += moveVal.x;
	obj->position.y += moveVal.y;
	obj->position.z += moveVal.z;

	obj->position.x = std::clamp(obj->position.x, -40.f, 40.f);
	obj->position.z = std::clamp(obj->position.z, -40.f, 40.f);

	const XMFLOAT2 angleDeg = GameObj::calcRotationSyncVelDeg(moveVal);
	this->setRotation(XMFLOAT3(angleDeg.x, angleDeg.y, this->getRotation().z));
}

void BaseActObj::attackProcess()
{
	if (!attackFlag) { return; }

	this->setCol(XMFLOAT4(1, 1, 1, this->getCol().w));
	if (++waitFrame >= 30)
	{
		attackFlag = false;
		waitFrame = 0;

		auto atkObj = atkObjPt.lock();
		const auto& atkCol = atkObj->getCol();
		atkObj->setCol(XMFLOAT4(1, 1, 1, atkCol.w));
	}
}