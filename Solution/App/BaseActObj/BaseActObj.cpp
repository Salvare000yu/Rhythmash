#include "BaseActObj.h"
#include <3D/ParticleMgr.h>

using namespace DirectX;

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos) :
	GameObj(camera, model, pos),
	particleMgr(std::make_unique<ParticleMgr>(L"Resources/white.png", camera))
{
	atkModel = std::make_unique<ObjModel>("Resources/Attack/", "Attack");

	atkObjPt = otherObj.emplace("AtkObj", std::make_unique<GameObj>(camera, atkModel.get())).first->second;

	damage = std::make_unique<Sound>("Resources/SE/damage.wav");

	auto atkObj = atkObjPt.lock();

	atkObj->setParent((BaseObj*)obj.get());
	atkObj->setPos(XMFLOAT3(0, 0, 5));
	atkObj->setCol(XMFLOAT4(1, 1, 1, 0.2f));

	atkcoll.group.emplace_front(CollisionMgr::ColliderType{ .obj = atkObj.get(), .colliderR = atkObj->getScaleF3().z });

	atkcoll.hitProc = [](GameObj* obj) {};
	mycoll.hitProc = [&](GameObj* obj)
	{
		ParticleMgr::createParticle(particleMgr.get(), this->calcWorldPos(), 50ui16);
		Sound::SoundPlayWave(damage.get());
		if (obj->damage(1ui16, true))
		{

			this->setCol(XMFLOAT4(0, 0, 0, 1));
			return;
		}
		this->setCol({ 1,0,0,1 });
	};

	additionalUpdateProc.emplace("BaseActorObj::particleMgr", [&] { particleMgr->update(); });
	additionalDrawProc.emplace("BaseActorObj::particleMgr", [&](Light*) { particleMgr->draw(); });
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
