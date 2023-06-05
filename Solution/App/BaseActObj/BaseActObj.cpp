#include "BaseActObj.h"

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos):
	GameObj(camera,model,pos)
{
	AtkModel = std::make_unique<ObjModel>("Resources/Attack/", "Attack");
	AtkObj = std::make_unique<GameObj>( camera, AtkModel.get());

	Atkcoll.group.emplace_front(CollisionMgr::ColliderType{ .obj = AtkObj.get(), .colliderR = AtkObj->getScaleF3().z});

	particlMgr = std::make_unique<ParticleMgr>();
	particlMgr->setCamera(camera);

	Mycoll.hitProc = [&](GameObj* obj)
	{
		this->setCol({ 1,0,0,1 });
		particlMgr->createParticle(particlMgr.get(), this->getPos(), 50);
		if (obj->damage(1u, false))
		{

			obj->kill();
			return;
		}
	};

	Atkcoll.hitProc = [&](GameObj* obj)
	{
	
		
	};
}
