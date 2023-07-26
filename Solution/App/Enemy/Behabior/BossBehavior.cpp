#include "BossBehavior.h"
#include <Enemy/BaseEnemy.h>
#include <BehaviorTree/Selector.h>

using namespace DirectX;

BossBehavior::BossBehavior(BaseEnemy* enemy) :
	EnemyBaseBehavior(enemy),
	moveVel(DirectX::XMVectorSet(0, 0, enemy->getSpeed(), 1))
{
	movePhase = std::make_unique<Selector>();
	movePhase->addChild(Task(std::bind(&BossBehavior::phase_squareMove, this)));

	jumpAttackPhase = std::make_unique<Sequencer>();
	jumpAttackPhase->addChild(Task([&] { /* 空中へ上昇する */ return NODE_RESULT::SUCCESS; }));
	jumpAttackPhase->addChild(Task([&] { /* （空中で）移動する */ return NODE_RESULT::SUCCESS; }));
	jumpAttackPhase->addChild(Task([&] { /* 少し留まる */ return NODE_RESULT::SUCCESS; }));
	jumpAttackPhase->addChild(Task([&] { /* 落ちてくる */ return NODE_RESULT::SUCCESS; }));

	mainPhase = std::make_unique<Sequencer>();
	mainPhase->addChild(*movePhase);
	mainPhase->addChild(*jumpAttackPhase);
}

NODE_RESULT BossBehavior::phase_squareMove()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	// 移動させる
	moveVel = XMVector3Normalize(moveVel) * enemy->getSpeed();
	enemy->moveProcess(moveVel);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= moveCountMax) { return NODE_RESULT::RUNNING; }

	// 指定カウント経過していたら、移動方向を回転し成功として終了
	moveVel = XMVector3Rotate(moveVel,
							  XMQuaternionRotationRollPitchYaw(0,
															   XM_PIDIV2,
															   0));

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}