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
	/// @brief 移動フェーズ
	std::unique_ptr<BaseComposite> squareMovePhase;

	DirectX::XMVECTOR moveVel{};

	/// @brief フェーズ内現在の拍数
	uint16_t nowPhaseCount = 0ui16;

	static constexpr uint16_t moveCountMax = 4ui16;

	std::unique_ptr<BaseComposite> jumpAttackPhase;

public:
	NODE_RESULT phase_squareMove();

	NODE_RESULT jumpAttack_rising();
	NODE_RESULT jumpAttack_moving();
	NODE_RESULT jumpAttack_falling();

public:
	BossBehavior(BaseEnemy* enemy);

	BossBehavior() :BossBehavior(nullptr) {}
};