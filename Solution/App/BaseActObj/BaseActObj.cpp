#include "BaseActObj.h"
#include <Sound/Sound.h>
#include <3D/Obj/ObjModel.h>
#include <3D/ParticleMgr.h>
#include <algorithm>

using namespace DirectX;

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos) :
	GameObj(camera, model, pos),
	judge([] { return true; })
{
	atkObjPt = otherObj.emplace("BaseActObj::AtkObj", std::make_unique<GameObj>(camera, nullptr)).first->second;

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

void BaseActObj::createAtkParticle()
{
	// パーティクルを出す
	auto p = particle.lock();
	auto atkObj = atkObjPt.lock();
	if (p && atkObj)
	{
		p->createParticle(atkObj->calcWorldPos(), 50U, 1.f, 0.0625f,
						  XMFLOAT3(1.f, 0.f, 1.f),
						  XMFLOAT3(0.f, 1.f, 1.f));
	}
}

void BaseActObj::moveProcess(const XMFLOAT3& dir)
{
	moveProcess(XMLoadFloat3(&dir));
}

void BaseActObj::moveProcess(const DirectX::XMVECTOR& dir)
{
	const auto moveValVec = XMVector3Normalize(dir) * moveSpeed;

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