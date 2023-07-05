#include "EnemyBehavior.h"
#include "BaseEnemy.h"
#include <DirectXMath.h>

using namespace DirectX;

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	Sequencer(), enemy(enemy)
{
	movePhase = std::make_unique<Selector>();
	movePhase->addChild(Task(std::bind(&EnemyBehavior::Phase_move, this)));

	attackPhase = std::make_unique<Selector>();
	attackPhase->addChild(Task([&] { return this->enemy->TargetFromDistance() < 10.0f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	attackPhase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack, this)));

	Phase = std::make_unique<Sequencer>();
	Phase->addChild(*movePhase);
	Phase->addChild(*attackPhase);

	addChild(*Phase);
	
	moveVelRotaQuaternion = XMQuaternionRotationRollPitchYaw(0, XM_PIDIV2, 0);
	
	//enemy->moveSpeed = enemy->enemy1Data.moveSpeed;
	
}




EnemyBehavior::EnemyBehavior() :
	EnemyBehavior(nullptr)
{}

void EnemyBehavior::drawImGui()
{}

NODE_RESULT EnemyBehavior::Phase_move()
{
	if (preBeatCount != enemy->nowBeatCount)
	{
		enemy->MoveProcess(moveVel);
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
	if (preBeatCount != enemy->nowBeatCount)
	{
		enemy->attackFlag = true;
		enemy->setCol(XMFLOAT4(0.5f, 1, 1, 1));
		if (++attackCount > attackCountMax)
		{
			enemy->attackFlag = false;
			attackCount = 0ui16;
			enemy->setCol(XMFLOAT4(1, 1, 1, 1));
			return NODE_RESULT::SUCCESS;
		}
	}

	return NODE_RESULT::RUNNING;
}

