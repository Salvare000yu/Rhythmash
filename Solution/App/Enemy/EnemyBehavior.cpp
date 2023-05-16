#include "EnemyBehavior.h"

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy):
	Selector(),enemy(enemy)
{
	nearTargetPhase = std::make_unique<Sequencer>();
	//nearTargetPhase->addChild(Task([&] {}));
	//nearTargetPhase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack)));
}
