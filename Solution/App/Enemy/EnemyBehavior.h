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

	//近いとき
	std::unique_ptr<Sequencer> nearTargetPhase;
	//遠い時
	std::unique_ptr<Sequencer> farTargetPhase;

	std::unique_ptr<Sequencer> movePhase;

	std::unique_ptr<Sequencer> attackPhase;

private://メンバ関数



	NODE_RESULT Phase_move();

	NODE_RESULT Phase_Attack();

public:
	EnemyBehavior(BaseEnemy* enemy);

	EnemyBehavior();
	//セット
	inline void SetEnemy(BaseEnemy* enemy) { this->enemy = enemy; }
};

