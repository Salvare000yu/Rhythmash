/*****************************************************************//**
 * \file   EnemyBaseBehavior.h
 * \brief  敵の行動基底クラス
 *********************************************************************/

#pragma once

#include <BehaviorTree/Sequencer.h>
#include <memory>

class BaseEnemy;

class EnemyBaseBehavior :
	public Sequencer
{
protected:
	/// @brief 敵へのポインタ
	BaseEnemy* enemy = nullptr;

	/// @brief 前フレームの拍数
	uint32_t preBeatCount = 0ui16;

	/// @brief 攻撃対象への距離
	float targetDistance = 0.f;

	/// @brief メインの行動
	std::unique_ptr<BaseComposite> mainPhase;

public:
	EnemyBaseBehavior(BaseEnemy* enemy);
	EnemyBaseBehavior() : EnemyBaseBehavior(nullptr) {};
};
