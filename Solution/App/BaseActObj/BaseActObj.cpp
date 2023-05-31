#include "BaseActObj.h"
#include <3D/ParticleMgr.h>

using namespace DirectX;

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos) :
	GameObj(camera, model, pos)
{
	atkModel = std::make_unique<ObjModel>("Resources/Attack/", "Attack");

	atkObjPt = otherObj.emplace("AtkObj", std::make_unique<GameObj>(camera, atkModel.get())).first->second;

	auto atkObj = atkObjPt.lock();

	atkObj->setParent((BaseObj*)obj.get());
	atkObj->setPos(XMFLOAT3(0, 0, 5));

	atkcoll.group.emplace_front(CollisionMgr::ColliderType{.obj = atkObj.get(), .colliderR = atkObj->getScaleF3().z });

	atkcoll.hitProc = [](GameObj* obj) {};
	mycoll.hitProc = [&](GameObj* obj)
	{
		if (obj->damage(1ui16, true))
		{
			this->setCol(XMFLOAT4(0, 0, 0, 1));
			return;
		}
		this->setCol({ 1,0,0,1 });
	};
}

void BaseActObj::MoveProcess(const XMFLOAT3& dir)
{
	MoveProcess(XMLoadFloat3(&dir));
}

void BaseActObj::MoveProcess(const DirectX::XMVECTOR& dir)
{
	const auto moveValVec = XMVector3Normalize(dir) * (moveSpeed / DX12Base::ins()->getFPS());

	XMFLOAT3 moveVal{};
	XMStoreFloat3(&moveVal, moveValVec);
	obj->position.x += moveVal.x;
	obj->position.y += moveVal.y;
	obj->position.z += moveVal.z;

	const XMFLOAT2 angleDeg = GameObj::calcRotationSyncVelDeg(moveVal);
	this->setRotation(XMFLOAT3(angleDeg.x, angleDeg.y, this->getRotation().z));
}

void BaseActObj::AttackProcess()
{
	if (!attackFlag) { return; }

	this->setCol({ 1,1,1,1 });
	if (++waitFrame >= 30)
	{
		attackFlag = false;
		atkObjPt.lock()->setCol({ 1,1,1,1 });
		waitFrame = 0;
	}
}
