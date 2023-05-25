#include "BaseEnemy.h"
#include <Util/RandomNum.h>
#include <BehaviorTree/Sequencer.h>

using namespace DirectX;

BaseEnemy::BaseEnemy(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos)
{
	this->setPos({ 20,0,0 });

	attackProc = std::make_unique<Sequencer>();
	attackProc->addChild(Task([&]
							  {
								  if (actFrame > 100ui16)
								  {
									  attackFlag = true;
									  return NODE_RESULT::SUCCESS;
								  }
								  ++actFrame;
								  return NODE_RESULT::RUNNING;
							  }));
	attackProc->addChild(Task([&]
							  {
								  Attack();

								  if (!attackFlag)
								  {
									  actFrame = 0ui16;
									  return NODE_RESULT::SUCCESS;
								  }
								  return NODE_RESULT::RUNNING;
							  }));
}

void BaseEnemy::additionalUpdate()
{
	Move();
	this->setCol({ 1,0,0,1 });
	
	attackProc->run();
}

void BaseEnemy::Move()
{
	if (moved)
	{
		if (++waitFrame >= 100)
		{
			moved = false;
		}
	} else
	{
		dir = XMFLOAT3(RandomNum::getRandf(minX, maxX), 0.0f, RandomNum::getRandf(minZ, maxZ));
		waitFrame = 0;
		moved = true;
	}
	MoveProcess(dir);
}

void BaseEnemy::Attack()
{
	AttackProcess();
	this->setCol({ 0,0,0,1 });
}
