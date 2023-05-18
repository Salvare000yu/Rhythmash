#include "EnemyBehavior.h"
#include "BaseEnemy.h"

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	Selector(), enemy(enemy)
{


	movePhase = std::make_unique<Sequencer>();
	movePhase->addChild(Task(std::bind(&EnemyBehavior::Phase_move, this)));

	attackPhase = std::make_unique<Sequencer>();
	attackPhase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack, this)));

	nearTargetPhase = std::make_unique<Sequencer>();
	//nearTargetPhase->addChild(Task([&] { return this->enemy->TargetFromDistance() > this->enemy->getMaxTargetDistance() * 0.5f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	nearTargetPhase->addChild(*attackPhase);


	farTargetPhase = std::make_unique<Sequencer>();
	//farTargetPhase->addChild(Task([&] { return this->enemy->TargetFromDistance() > this->enemy->getMaxTargetDistance() * 0.5f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	farTargetPhase->addChild(*movePhase);
	farTargetPhase->addChild(*attackPhase);
	//addChild(*nearTargetPhase);
	addChild(*farTargetPhase);
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

