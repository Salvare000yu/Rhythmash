#include <Enemy/Behabior/EnemyBehavior.h>
#include <Enemy/BaseEnemy.h>
#include <BehaviorTree/Selector.h>
#include <DirectXMath.h>

using namespace DirectX;

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	EnemyBaseBehavior(enemy)
{
	movePhase = std::make_unique<Selector>();
	movePhase->addChild(Task(std::bind(&EnemyBehavior::phase_move, this)));

	attackPhase = std::make_unique<Sequencer>();
	attackPhase->addChild(Task([&] { return this->enemy->TargetFromDistance() < 10.0f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	attackPhase->addChild(Task(std::bind(&EnemyBehavior::phase_Attack, this)));

	mainPhase = std::make_unique<Sequencer>();
	mainPhase->addChild(*movePhase);
	mainPhase->addChild(*attackPhase);

	enemy->setSpeed(BaseActObj::moveSpeedDef * 10.f);

	moveVel = DirectX::XMVectorSet(0, 0, enemy->getSpeed(), 1);
	moveVelRotaQuaternion = XMQuaternionRotationRollPitchYaw(0, XM_PIDIV2, 0);
}

NODE_RESULT EnemyBehavior::phase_move()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	// 移動させる
	moveVel = XMVector3Normalize(moveVel) * enemy->getSpeed();
	enemy->moveProcess(moveVel);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= moveCountMax) { return NODE_RESULT::RUNNING; }

	// 指定カウント経過していたら、移動方向を回転し成功として終了
	moveVel = XMVector3Rotate(moveVel, moveVelRotaQuaternion);

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT EnemyBehavior::phase_Attack()
{
	// カウントが変わっていなければ、実行中として終了
	if (preBeatCount == enemy->getNowBeatCount()) { return NODE_RESULT::RUNNING; }

	// カウントが変わった瞬間なら、攻撃状態に切り替える
	enemy->setAttackFlag(true);

	// 指定カウント経過していなければ、実行中として終了
	if (++nowPhaseCount <= attackCountMax) { return NODE_RESULT::RUNNING; }

	// 指定カウント経過していたら、値を戻し成功として終了
	enemy->setAttackFlag(false);

	nowPhaseCount = 0ui16;
	return NODE_RESULT::SUCCESS;
}