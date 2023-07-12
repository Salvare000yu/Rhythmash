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

 /// @brief 自機クラス
class Player
	: public BaseActObj
{
	/// @brief 効果音の参照変数
	std::weak_ptr<SoundData> se1;

	/// @brief 更新処理で行う内容
	std::function<void()> update_proc;

	/// @brief 攻撃状態の現在の経過時間
	uint32_t attackFrame = 0;

	/// @brief 通常移動の速さ
	float normalSpeed = BaseActObj::moveSpeedDef;

	/// @brief ダッシュ移動の速さ
	float dashSpeed = BaseActObj::moveSpeedDef;

	/// @brief ダッシュ速度の減衰値
	float dashSpeedAttenuation = 1.f;

private:
	/// @brief データをYAMLファイルから読み込む
	/// @return エラーがあったかどうか（エラーでtrue）
	bool loadYamlFile();

	/// @brief 自機の攻撃処理
	void attack() override;

	/// @brief ダッシュ
	void step();

public:
	/// @brief コンストラクタ
	Player(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });
};
