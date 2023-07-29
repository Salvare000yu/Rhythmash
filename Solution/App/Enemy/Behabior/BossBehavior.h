/*****************************************************************//**
 * \file   EnemyBehavior.h
 * \brief  ボスの行動クラス
 *********************************************************************/

#pragma once

#include <Enemy/EnemyBaseBehavior.h>
#include <DirectXMath.h>

class Timer;

class BossBehavior :
	public EnemyBaseBehavior
{
private:
	std::weak_ptr<Timer> enemyTimerRef;

	/// @brief 移動フェーズ
	std::unique_ptr<BaseComposite> squareMovePhase;

	DirectX::XMVECTOR moveVel{};

	/// @brief フェーズ内現在の拍数
	uint16_t nowPhaseCount{};

	std::unique_ptr<BaseComposite> jumpAttackPhase;
	std::unique_ptr<Timer> phaseTimer;
public:
	NODE_RESULT phase_squareMove();

	NODE_RESULT jumpAttack_rising(bool downFlag);
	NODE_RESULT jumpAttack_moving();

public:
	BossBehavior(BaseEnemy* enemy);

	BossBehavior() :BossBehavior(nullptr) {}
};