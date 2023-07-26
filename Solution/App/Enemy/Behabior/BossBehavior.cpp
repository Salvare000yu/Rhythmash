#include "BossBehavior.h"
#include <Enemy/BaseEnemy.h>
#include <BehaviorTree/Selector.h>

using namespace DirectX;

namespace
{
	constexpr uint16_t squareMoveCountMax = 4ui16;

	constexpr uint16_t risingCountMax = 4ui16;
	constexpr uint16_t movingCountMax = 8ui16;
}

BossBehavior::BossBehavior(BaseEnemy* enemy) :
	EnemyBaseBehavior(enemy),
	nowPhaseCount(0ui16),
	phaseTimer(std::make_unique<Timer>())
{
	moveVel = XMVectorSet(0, 0, enemy->getSpeed(), 1);
	enemyTimerRef = enemy->getTimerRef();

	squareMovePhase = std::make_unique<Selector>();
	squareMovePhase->addChild(Task(std::bind(&BossBehavior::phase_squareMove, this)));

	// ジャンプ攻撃時のY座標の最小値と最大値
	constexpr float jumpAttackPosMax = 20.f;
	constexpr float jumpAttackPosMin = 0.f;

	jumpAttackPhase = std::make_unique<Sequencer>();
	jumpAttackPhase->addChild(Task([&]
								   {
									   auto timer = this->enemy->getTimerRef().lock();
									   if (!timer) { return NODE_RESULT::FAIL; }
									   timer->reset();
									   return NODE_RESULT::SUCCESS;
								   }));
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_rising, this, jumpAttackPosMin, jumpAttackPosMax)));
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_moving, this)));
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_rising, this, jumpAttackPosMax, jumpAttackPosMin)));

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
	if (nowPhaseCount < squareMoveCountMax) { ++nowPhaseCount; return NODE_RESULT::RUNNING; }

	// 指定カウント経過していたら、移動方向を回転し成功として終了
	moveVel = XMVector3Rotate(moveVel,
							  XMQuaternionRotationRollPitchYaw(0,
															   XM_PIDIV2,
															   0));

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT BossBehavior::jumpAttack_rising(float startPosY, float endPosY)
{
	const auto timer = enemyTimerRef.lock();
	if (!timer) { return NODE_RESULT::FAIL; }

	const auto risingTimeMax = (risingCountMax + 1ui16) * timer->calc1BeatTime(timer->bpm);

	const float raito = float(timer->getNowTime()) / float(risingTimeMax);

	XMFLOAT3 pos = enemy->getPos();
	pos.y = std::lerp(startPosY, endPosY, raito);
	enemy->setPos(pos);

	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	// 指定カウント経過していなければ、実行中として終了
	if (nowPhaseCount <= risingCountMax) { ++nowPhaseCount; return NODE_RESULT::RUNNING; }

	pos.y = endPosY;
	enemy->setPos(pos);

	nowPhaseCount = 0ui16;
	timer->reset();
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT BossBehavior::jumpAttack_moving()
{
	const auto timer = enemyTimerRef.lock();
	if (!timer) { return NODE_RESULT::FAIL; }

	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	XMVECTOR moveVec = XMLoadFloat3(&enemy->getTargetWorldPos());
	moveVec -= XMLoadFloat3(&enemy->calcWorldPos());
	moveVec = XMVectorSetY(moveVec, 0.f);

	enemy->moveProcess(moveVec);

	// 指定カウント経過していなければ、実行中として終了
	if (nowPhaseCount <= movingCountMax) { ++nowPhaseCount; return NODE_RESULT::RUNNING; }

	nowPhaseCount = 0ui16;
	timer->reset();
	return NODE_RESULT::SUCCESS;
}