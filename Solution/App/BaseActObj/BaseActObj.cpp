#include "BaseActObj.h"

BaseActObj::BaseActObj(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos):
	GameObj(camera,model,pos)
{
	AtkModel = std::make_unique<ObjModel>("Resources/Attack/", "Attack");
	AtkObj = std::make_unique<GameObj>( camera, AtkModel.get());

	Atkcoll.group.emplace_front(CollisionMgr::ColliderType{ .obj = AtkObj.get(), .colliderR = AtkObj->getScaleF3().z});

	Atkcoll.hitProc = [&](GameObj* obj)
	{
	
		
	};
}
