/*****************************************************************//**
 * \file   EnemyBehavior.h
 * \brief  敵の行動クラス
 *********************************************************************/

#pragma once

#include <Enemy/EnemyBaseBehavior.h>
#include <DirectXMath.h>

class BaseEnemy;

/// @brief 通常敵の行動クラス
class EnemyBehavior :
	public EnemyBaseBehavior
{
private:
	/// @brief 移動フェーズ
	std::unique_ptr<BaseComposite> squareMovePhase;

	/// @brief 攻撃フェーズ
	std::unique_ptr<BaseComposite> attackPhase;

	/// @brief 接近して攻撃するフェーズ
	std::unique_ptr<BaseComposite> approachAttackPhase;

	/// @brief フェーズ内現在の拍数
	uint16_t nowPhaseCount = 0ui16;

	static constexpr uint16_t moveCountMax = 4ui16;
	static constexpr uint16_t attackCountMax = 4ui16;

	DirectX::XMVECTOR squareMoveVel{};
	DirectX::XMVECTOR squareMoveVelRotaQuaternion{};

private:
	NODE_RESULT phase_squareMove();
	NODE_RESULT phase_Attack();
	NODE_RESULT phase_Approach();

public:
	EnemyBehavior(BaseEnemy* enemy);

	EnemyBehavior() : EnemyBehavior(nullptr) {};
};
