#include "BossBehavior.h"
#include <Enemy/BaseEnemy.h>
#include <BehaviorTree/Selector.h>

using namespace DirectX;

namespace
{
	constexpr uint16_t risingCountMax = 8ui16;
	constexpr uint16_t movingCountMax = 8ui16;
}

BossBehavior::BossBehavior(BaseEnemy* enemy) :
	EnemyBaseBehavior(enemy),
	moveVel(DirectX::XMVectorSet(0, 0, enemy->getSpeed(), 1))
{
	squareMovePhase = std::make_unique<Selector>();
	squareMovePhase->addChild(Task(std::bind(&BossBehavior::phase_squareMove, this)));

	jumpAttackPhase = std::make_unique<Sequencer>();
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_rising, this)));
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_moving, this)));
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_falling, this)));

	mainPhase = std::make_unique<Sequencer>();
	mainPhase->addChild(*squareMovePhase);
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

NODE_RESULT BossBehavior::jumpAttack_rising()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	enemy->moveProcess(XMVectorSet(0, enemy->getSpeed(), 0, 1), false);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= risingCountMax) { return NODE_RESULT::RUNNING; }

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT BossBehavior::jumpAttack_moving()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	XMVECTOR moveVec = XMLoadFloat3(&enemy->getTargetWorldPos());
	moveVec -= XMLoadFloat3(&enemy->calcWorldPos());
	moveVec = XMVectorSetY(moveVec, 0.f);

	enemy->moveProcess(moveVec);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= movingCountMax) { return NODE_RESULT::RUNNING; }

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT BossBehavior::jumpAttack_falling()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	enemy->moveProcess(XMVectorSet(0, -1, 0, 1), false);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= risingCountMax) { return NODE_RESULT::RUNNING; }

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}
