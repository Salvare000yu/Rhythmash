#pragma once
#include "BaseActObj/BaseActObj.h"
#include "EnemyBehavior.h"
#include "Collision/Collision.h"

class BaseEnemy : public BaseActObj
{
	friend class EnemyBehavior;

	std::unique_ptr<EnemyBehavior> enemyBehavior;

	std::optional<EnemyData> loadYamlFile();


	EnemyData enemy1Data;
	EnemyData enemy2Data;
	EnemyData bossData;
protected:
	GameObj* targetObj = nullptr;
	uint32_t nowBeatCount = 0ui16;

public:
	BaseEnemy(Camera* camera,
			  ObjModel* model,
			  const DirectX::XMFLOAT3& pos = { 0, 0, 0 });

	inline void setNowBeatCount(uint32_t count) { nowBeatCount = count; }
	inline uint32_t getNowBeatCount() const { return nowBeatCount; }

	inline void setTargetObj(GameObj* obj) { targetObj = obj; }

	float TargetFromDistance();
	void drawIdmGui();
};
