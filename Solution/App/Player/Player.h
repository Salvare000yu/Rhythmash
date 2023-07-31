/*****************************************************************//**
 * \file   Player.h
 * \brief  自機クラス
 *********************************************************************/

#pragma once

#include "BaseActObj/BaseActObj.h"
#include <DirectXMath.h>
#include <functional>
#include <Sound/Sound.h>
#include <memory>

class Timer;
class ObjModel;

/// @brief 自機クラス
class Player
	: public BaseActObj
{
	/// @brief 効果音の参照変数
	std::weak_ptr<SoundData> se1;

	/// @brief 更新処理で行う内容
	std::function<void()> update_proc;

	/// @brief 攻撃状態の現在の経過時間
	std::unique_ptr<Timer> attackTimer;

	/// @brief 通常移動の速さ
	float normalSpeed = BaseActObj::moveSpeedDef;

	/// @brief ダッシュ移動の速さ
	float dashSpeed = BaseActObj::moveSpeedDef;

	/// @brief ダッシュ速度の減衰値
	float dashSpeedAttenuation = 1.f;

	/// @brief 攻撃しているかどうか
	bool attackFlag = false;

	std::unique_ptr<GameObj> attackObj = nullptr;
	std::unique_ptr<ObjModel> attackModel = nullptr;

	CollisionMgr::ColliderType attackObjCol{};
	CollisionMgr::ColliderSet attackObjColSet{};

private:
	/// @brief データをYAMLファイルから読み込む
	/// @return エラーがあったかどうか（エラーでtrue）
	bool loadYamlFile();

	/// @brief 自機の攻撃処理
	void attack() override;

	/// @brief ダッシュ
	void step();

	void startAttack();

public:
	/// @brief コンストラクタ
	Player(Camera* camera,
		   ObjModel* model,
		   std::weak_ptr<Timer> timer,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });

	inline bool getAttackFlag() const { return attackFlag; }

	inline const auto& getAttackObjCollider() const { return attackObjCol; }
	inline const auto& getAttackObjColliderSet() const { return attackObjColSet; }
};
