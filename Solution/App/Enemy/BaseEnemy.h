/*****************************************************************//**
 * \file   BaseEnemy.h
 * \brief  敵基底クラス
 *********************************************************************/

#pragma once

#include "BaseActObj/BaseActObj.h"
#include <BehaviorTree/BaseComposite.h>
#include "EnemyMgr.h"

 /// @brief 敵基底クラス
class BaseEnemy :
	public BaseActObj
{
	std::unique_ptr<BaseComposite> enemyBehavior;

protected:
	// 攻撃対象へのポインタ
	GameObj* targetObj = nullptr;

	uint32_t nowBeatCount = 0ui16;

public:
	BaseEnemy(Camera* camera,
			  ObjModel* model,
			  const DirectX::XMFLOAT3& pos = { 0,0,0 });

	template<class Ty>
	inline void setBehavior() { enemyBehavior = std::make_unique<Ty>(); }

	inline void setBehavior(std::unique_ptr<BaseComposite>&& proc) { enemyBehavior = std::move(proc); }

	inline void setNowBeatCount(uint32_t count) { nowBeatCount = count; }
	inline uint32_t getNowBeatCount() const { return nowBeatCount; }

	inline void setTargetObj(GameObj* obj) { targetObj = obj; }

	//ターゲットとの距離間
	float TargetFromDistance();
};
