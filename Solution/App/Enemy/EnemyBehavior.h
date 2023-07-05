#pragma once
#include <BehaviorTree/Selector.h>
#include <BehaviorTree/Sequencer.h>
#include <memory>
#include <DirectXMath.h>

class BaseEnemy;

class EnemyBehavior :
	public Sequencer
{
private://メンバ変数

	BaseEnemy* enemy = nullptr;

	std::unique_ptr<Sequencer> mainPhase;

	std::unique_ptr<Selector> movePhase;
	std::unique_ptr<Selector> attackPhase;

	static constexpr uint16_t moveCountMax = 4ui16;
	uint16_t moveCount = 0ui16;

	static constexpr uint16_t attackCountMax = 4ui16;
	uint16_t attackCount = 0ui16;

	uint32_t preBeatCount = 0ui16;

	DirectX::XMVECTOR moveVel = DirectX::XMVectorSet(0, 0, 10, 1);
	DirectX::XMVECTOR moveVelRotaQuaternion{};

private://メンバ関数

	NODE_RESULT Phase_move();

	NODE_RESULT Phase_Attack();

public:
	EnemyBehavior(BaseEnemy* enemy);

	EnemyBehavior() : EnemyBehavior(nullptr) {};
};
