#include <Enemy/Behabior/EnemyBehavior.h>
#include <Enemy/BaseEnemy.h>
#include <BehaviorTree/Selector.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace
{
	constexpr float startApproachDistance = 30.f;
}

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	EnemyBaseBehavior(enemy)
{
	squareMovePhase = std::make_unique<Sequencer>();
	squareMovePhase->addChild(Task([&] { return this->targetDistance > startApproachDistance ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	squareMovePhase->addChild(Task(std::bind(&EnemyBehavior::phase_squareMove, this)));

	approachAttackPhase = std::make_unique<Selector>();
	approachAttackPhase->addChild(Task(std::bind(&EnemyBehavior::phase_Approach, this)));

	mainPhase = std::make_unique<Selector>();
	mainPhase->addChild(*squareMovePhase);
	mainPhase->addChild(*approachAttackPhase);

	squareMoveVel = DirectX::XMVectorSet(0, 0, enemy->getSpeed(), 1);
	squareMoveVelRotaQuaternion = XMQuaternionRotationRollPitchYaw(0, XM_PIDIV2, 0);
}

NODE_RESULT EnemyBehavior::phase_squareMove()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	// 移動させる
	squareMoveVel = XMVector3Normalize(squareMoveVel);
	enemy->moveProcess(squareMoveVel);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= moveCountMax) { return NODE_RESULT::RUNNING; }

	// 指定カウント経過していたら、移動方向を回転し成功として終了
	squareMoveVel = XMVector3Rotate(squareMoveVel, squareMoveVelRotaQuaternion);

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT EnemyBehavior::phase_Approach()
{
	// 追尾開始距離より遠ければ失敗とする
	if (targetDistance > startApproachDistance) { return NODE_RESULT::FAIL; }

	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	XMVECTOR moveVec = XMLoadFloat3(&enemy->getTargetWorldPos());
	moveVec -= XMLoadFloat3(&enemy->calcWorldPos());

	enemy->moveProcess(moveVec);

	return NODE_RESULT::RUNNING;
}