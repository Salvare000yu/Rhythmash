#include "BaseActObj.h"
#include <Sound/Sound.h>
#include <3D/Obj/ObjModel.h>
#include <3D/ParticleMgr.h>
#include <algorithm>

using namespace DirectX;

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, std::weak_ptr<Timer> timer, const DirectX::XMFLOAT3& pos) :
	GameObj(camera, model, pos),
	timerRef(timer),
	judge([] { return true; })
{
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

void BaseActObj::moveProcess(const XMFLOAT3& dir, bool changeRotationFlag)
{
	moveProcess(XMLoadFloat3(&dir), changeRotationFlag);
}

void BaseActObj::moveProcess(const DirectX::XMVECTOR& dir, bool changeRotationFlag)
{
	const auto moveValVec = XMVector3Normalize(dir) * moveSpeed;

	XMFLOAT3 moveVal{};
	XMStoreFloat3(&moveVal, moveValVec);
	obj->position.x += moveVal.x;
	obj->position.y += moveVal.y;
	obj->position.z += moveVal.z;

	obj->position.x = std::clamp(obj->position.x, -40.f, 40.f);
	obj->position.z = std::clamp(obj->position.z, -40.f, 40.f);

	if (changeRotationFlag)
	{
		const XMFLOAT2 angleDeg = GameObj::calcRotationSyncVelDeg(moveVal);
		this->setRotation(XMFLOAT3(angleDeg.x, angleDeg.y, this->getRotation().z));
	}
}