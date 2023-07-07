#include "EnemyBehavior.h"
#include "BaseEnemy.h"
#include <DirectXMath.h>

using namespace DirectX;

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	Sequencer(), enemy(enemy)
{
	movePhase = std::make_unique<Selector>();
	movePhase->addChild(Task(std::bind(&EnemyBehavior::Phase_move, this)));

	attackPhase = std::make_unique<Sequencer>();
	attackPhase->addChild(Task([&] { return this->enemy->TargetFromDistance() < 10.0f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	attackPhase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack, this)));

	mainPhase = std::make_unique<Sequencer>();
	mainPhase->addChild(*movePhase);
	mainPhase->addChild(*attackPhase);

	addChild(Task([&]
				  {
					  // メインの行動をし、結果を取得
					  const NODE_RESULT ret = mainPhase->run();

					  // 拍数を更新
					  preBeatCount = this->enemy->getNowBeatCount();

					  // 行動の結果を返す
					  return ret;
				  }));

	moveVelRotaQuaternion = XMQuaternionRotationRollPitchYaw(0, XM_PIDIV2, 0);

	enemy->setSpeed(BaseActObj::moveSpeedDef * 10.f);
}

NODE_RESULT EnemyBehavior::Phase_move()
{
	if (preBeatCount != enemy->getNowBeatCount())
	{
		enemy->moveProcess(moveVel);
		if (++moveCount > moveCountMax)
		{
			moveVel = XMVector3Rotate(moveVel, moveVelRotaQuaternion);

			moveCount = 0ui16;
			return NODE_RESULT::SUCCESS;
		}
	}

	return NODE_RESULT::RUNNING;
}

NODE_RESULT EnemyBehavior::Phase_Attack()
{
	if (preBeatCount != enemy->getNowBeatCount())
	{
		enemy->setAttackFlag(true);
		enemy->setCol(XMFLOAT4(0.5f, 1, 1, 1));
		if (++attackCount > attackCountMax)
		{
			enemy->setAttackFlag(false);
			attackCount = 0ui16;
			enemy->setCol(XMFLOAT4(1, 1, 1, 1));
			return NODE_RESULT::SUCCESS;
		}
	}

	return NODE_RESULT::RUNNING;
}