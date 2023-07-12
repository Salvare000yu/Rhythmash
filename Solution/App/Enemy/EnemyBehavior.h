/*****************************************************************//**
 * \file   EnemyBehavior.h
 * \brief  敵の行動クラス
 *********************************************************************/

#pragma once

#include <BehaviorTree/Sequencer.h>
#include <memory>
#include <DirectXMath.h>

class BaseEnemy;

/// @brief 通常敵の行動クラス
class EnemyBehavior :
	public Sequencer
{
	//メンバ変数
private:
	/// @brief 敵へのポインタ
	BaseEnemy* enemy = nullptr;

	/// @brief メインの行動
	std::unique_ptr<BaseComposite> mainPhase;

	/// @brief 移動フェーズ
	std::unique_ptr<BaseComposite> movePhase;

	/// @brief 攻撃フェーズ
	std::unique_ptr<BaseComposite> attackPhase;

	/// @brief フェーズ内現在の拍数
	uint16_t nowPhaseCount = 0ui16;

	static constexpr uint16_t moveCountMax = 4ui16;
	static constexpr uint16_t attackCountMax = 4ui16;

	/// @brief 前フレームの拍数
	uint32_t preBeatCount = 0ui16;

	DirectX::XMVECTOR moveVel = DirectX::XMVectorSet(0, 0, 10, 1);
	DirectX::XMVECTOR moveVelRotaQuaternion{};

private://メンバ関数

	NODE_RESULT phase_move();

	NODE_RESULT phase_Attack();

public:
	EnemyBehavior(BaseEnemy* enemy);

	EnemyBehavior() : EnemyBehavior(nullptr) {};
};
