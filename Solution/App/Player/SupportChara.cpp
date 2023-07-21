#include "SupportChara.h"
using namespace DirectX;

SupportChara::SupportChara(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos), particleMgr(std::make_unique<ParticleMgr>(L"Resources/white.png", camera))
{
	additionalUpdateProc.emplace("SupportChara::particleMgr", [&] { particleMgr->update(); });
	additionalDrawProc.emplace("SupportChara::particleMgr", [&](Light*) { particleMgr->draw(); });
}

void SupportChara::Move()
{
	dir = { 0,0,0 };

	if (judge())
	{
		Step();
		ParticleMgr::createParticle(particleMgr.get(), this->calcWorldPos(),10U,1.0f,2.f,
								{(0.0f),(1.0f),(1.0f)}, { (0.0f),(1.0f),(1.0f) });
	}
	
}

void SupportChara::Step()
{
	if (Rev==true)
	{
		dir.y = 1;
		Rev = false;
		MoveProcess(dir);
		return;
	}
	else if(Rev == false)
	{
		dir.y = -1;
		Rev = true;
		MoveProcess(dir);
		return;
	}

}

