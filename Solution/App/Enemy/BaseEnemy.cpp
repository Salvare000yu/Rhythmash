#include "BaseEnemy.h"
#include <Util/RandomNum.h>
#include <BehaviorTree/Sequencer.h>
#include <Util/RandomNum.h>

using namespace DirectX;

BaseEnemy::BaseEnemy(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos),
	enemyBehavior(std::make_unique<EnemyBehavior>(this))
{
	additionalUpdateProc.emplace("BaseEnemy::update_proc", [&] { enemyBehavior->run(); });
	additionalUpdateProc.emplace("BaseEnemy::カウント更新", [&] { enemyBehavior->setPreBeatCount(nowBeatCount); });
}

float BaseEnemy::TargetFromDistance()
{
	if (!this->targetObj) { return -1.f; }

	const XMVECTOR pos = XMLoadFloat3(&this->calcWorldPos());
	const XMVECTOR tpos = XMLoadFloat3(&this->targetObj->calcWorldPos());

	return Collision::vecLength(pos - tpos);
}

void BaseEnemy::drawIdmGui()
{
	enemyBehavior->drawImGui();
}
