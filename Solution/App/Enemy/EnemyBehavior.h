﻿#pragma once
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

	std::unique_ptr<Sequencer> Phase;

	std::unique_ptr<BaseComposite> movePhase;
	std::unique_ptr<BaseComposite> attackPhase;

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
	inline void setPreBeatCount(uint32_t count) { preBeatCount = count; };

	EnemyBehavior(BaseEnemy* enemy);

	EnemyBehavior();

	void drawImGui();

};

