#pragma once
#include <BehaviorTree/Selector.h>
#include <BehaviorTree/Sequencer.h>
#include <memory>
#include <DirectXMath.h>

class BaseEnemy;

class EnemyBehavior :
    public Selector
{
private://メンバ変数

	BaseEnemy* enemy = nullptr;

	
	std::unique_ptr<Sequencer> Phase;
	

	std::unique_ptr<Selector> movePhase;

	std::unique_ptr<Selector> attackPhase;

private://メンバ関数

	NODE_RESULT Phase_move();

	NODE_RESULT Phase_Attack();

public:
	EnemyBehavior(BaseEnemy* enemy);

	EnemyBehavior();
	
};

