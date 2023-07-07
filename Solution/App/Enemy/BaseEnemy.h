#pragma once
#include "BaseActObj/BaseActObj.h"
#include "EnemyBehavior.h"
#include "Collision/Collision.h"
struct EnemyData
{
	float moveSpeed;
	float dashSpeed;
	float dashSpeedAttenuation;
	uint16_t hp;
	uint16_t attack;
};

class BaseEnemy : public BaseActObj
{
	friend class EnemyBehavior;

	std::unique_ptr<EnemyBehavior> enemyBehavior;

	


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
	
	
	inline uint16_t getHp(int number)
	{
		switch (number)
		{
		case 1:
			return enemy1Data.hp;
		case 2:
			return enemy2Data.hp;
		case 3:
			return bossData.hp;
		default:
			return 0;
		};
	 }	
	inline uint16_t getAttack(int number)
	{
		switch (number)
		{
		case 1:
			return enemy1Data.attack;
		case 2:
			return enemy2Data.attack;
		case 3:
			return bossData.attack;
		default:
			return 0;
		};
	 }inline float getmoveSpeed(int number)
	{
		switch (number)
		{
		case 1:
			return enemy1Data.moveSpeed;
		case 2:
			return enemy2Data.moveSpeed;
		case 3:
			return bossData.moveSpeed;
		default:
			return 0;
		};
	 }inline float getdashSpeed(int number)
	{
		switch (number)
		{
		case 1:
			return enemy1Data.moveSpeed * enemy1Data.dashSpeed;
		case 2:
			return enemy2Data.moveSpeed * enemy2Data.dashSpeed;
		case 3:
			return bossData.moveSpeed * bossData.dashSpeed;
		default:
			return 0;
		};
	 }inline float getdashSpeedAttenuation(int number)
	{
		switch (number)
		{
		case 1:
			return -enemy1Data.moveSpeed * enemy1Data.dashSpeedAttenuation;
		case 2:
			return -enemy2Data.moveSpeed * enemy2Data.dashSpeedAttenuation;
		case 3:
			return bossData.moveSpeed * bossData.dashSpeedAttenuation;
		default:
			return 0;
		};
	 }
	

	float TargetFromDistance();
	void drawIdmGui();

	void loadEnemyData();	
	EnemyData enemy1Data;
	EnemyData enemy2Data;
	EnemyData bossData;


};
