#include "BossBehavior.h"
#include <Enemy/BaseEnemy.h>
#include <BehaviorTree/Selector.h>

using namespace DirectX;

namespace
{
	constexpr uint16_t squareMoveCountMax = 4ui16;

	constexpr uint16_t risingCountMax = 2ui16;
	constexpr uint16_t movingCountMax = 4ui16;

	// ジャンプ攻撃時のY座標の幅
	constexpr float jumpAttackHeight = 20.f;

	constexpr float approachDistanceMin = 30.f;
}

BossBehavior::BossBehavior(BaseEnemy* enemy) :
	EnemyBaseBehavior(enemy),
	nowPhaseCount(0ui16),
	phaseTimer(std::make_unique<Timer>())
{
	moveVel = XMVectorSet(0, 0, enemy->getSpeed(), 1);
	enemyTimerRef = enemy->getTimerRef();

	// 正方形に動くフェーズ
	squareMovePhase = std::make_unique<Sequencer>();
	squareMovePhase->addChild(Task(std::bind(&BossBehavior::phase_squareMove, this)));

	// 遠ければ接近するフェーズ
	moveFieldPhase = std::make_unique<Sequencer>();
	moveFieldPhase->addChild(Task([&] { return this->enemy->targetFromDistance() > approachDistanceMin ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	moveFieldPhase->addChild(Task(std::bind(&BossBehavior::phase_moving, this)));

	movePhase = std::make_unique<Selector>();
	movePhase->addChild(*moveFieldPhase);
	movePhase->addChild(*squareMovePhase);

	const Task resetTimer = Task([&]
								   {
									   auto timer = this->enemy->getTimerRef().lock();
									   if (!timer) { return NODE_RESULT::FAIL; }
									   timer->reset();
									   return NODE_RESULT::SUCCESS;
								   });

	// 上昇し上から来るフェーズ
	jumpAttackPhase = std::make_unique<Sequencer>();
	jumpAttackPhase->addChild(resetTimer);
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_rising, this, false)));
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::phase_moving, this)));
	jumpAttackPhase->addChild(resetTimer);
	jumpAttackPhase->addChild(Task(std::bind(&BossBehavior::jumpAttack_rising, this, true)));

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
	if (nowPhaseCount < squareMoveCountMax) { ++nowPhaseCount; return NODE_RESULT::RUNNING; }

	// 指定カウント経過していたら、移動方向を回転し成功として終了
	moveVel = XMVector3Rotate(moveVel,
							  XMQuaternionRotationRollPitchYaw(0,
															   XM_PIDIV2,
															   0));

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT BossBehavior::jumpAttack_rising(bool downFlag)
{
	const auto timer = enemyTimerRef.lock();
	if (!timer) { return NODE_RESULT::FAIL; }

	const auto risingTimeMax = risingCountMax * timer->calc1BeatTime(timer->bpm);

	float raito = float(timer->getNowTime()) / float(risingTimeMax);
	raito *= raito * raito * raito;

	float startPosY = enemy->getScaleF3().y, endPosY = enemy->getScaleF3().y;
	if (downFlag)
	{
		startPosY += jumpAttackHeight;
	} else
	{
		endPosY += jumpAttackHeight;
	}

	XMFLOAT3 pos = enemy->getPos();
	pos.y = std::lerp(startPosY, endPosY, raito);
	enemy->setPos(pos);

	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	// 指定カウント経過していなければ、実行中として終了
	if (nowPhaseCount <= risingCountMax) { ++nowPhaseCount; return NODE_RESULT::RUNNING; }

	// 終わっていたら終了時の位置に合わせる
	pos.y = endPosY;
	enemy->setPos(pos);

	nowPhaseCount = 0ui16;
	timer->reset();
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT BossBehavior::phase_moving()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	XMVECTOR moveVec = XMLoadFloat3(&enemy->getTargetWorldPos());
	moveVec -= XMLoadFloat3(&enemy->calcWorldPos());
	moveVec = XMVectorSetY(moveVec, 0.f);

	enemy->moveProcess(moveVec);

	// 指定カウント経過していなければ、実行中として終了
	if (nowPhaseCount <= movingCountMax) { ++nowPhaseCount; return NODE_RESULT::RUNNING; }

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}