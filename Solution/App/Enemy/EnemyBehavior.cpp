#include "EnemyBehavior.h"
#include "BaseEnemy.h"

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	Selector(), enemy(enemy)
{


	//movePhase = std::make_unique<Selector>();
	//movePhase->addChild(Task(std::bind(&EnemyBehavior::Phase_move, this)));

	//attackPhase = std::make_unique<Selector>();
	//attackPhase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack, this)));


	Phase = std::make_unique<Sequencer>();
	Phase->addChild(Task(std::bind(&EnemyBehavior::Phase_move, this)));
	Phase->addChild(Task([&] { return this->enemy->TargetFromDistance() < 10.0f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	Phase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack, this)));

	addChild(*Phase);
}

EnemyBehavior::EnemyBehavior() :
	EnemyBehavior(nullptr)
{}

NODE_RESULT EnemyBehavior::Phase_move()
{
	this->enemy->Move();
	return NODE_RESULT::SUCCESS;
}

NODE_RESULT EnemyBehavior::Phase_Attack()
{
	this->enemy->Attack();
	return NODE_RESULT::SUCCESS;
}

