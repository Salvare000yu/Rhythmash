#include "EnemyBehavior.h"
#include "BaseEnemy.h"
#include <DirectXMath.h>

#include <fstream>
#include <Yaml.hpp>
using namespace DirectX;

EnemyBehavior::EnemyBehavior(BaseEnemy* enemy) :
	Sequencer(), enemy(enemy)
{
	movePhase = std::make_unique<Selector>();
	movePhase->addChild(Task(std::bind(&EnemyBehavior::Phase_move, this)));

	attackPhase = std::make_unique<Selector>();
	attackPhase->addChild(Task([&] { return this->enemy->TargetFromDistance() < 10.0f ? NODE_RESULT::SUCCESS : NODE_RESULT::FAIL; }));
	attackPhase->addChild(Task(std::bind(&EnemyBehavior::Phase_Attack, this)));

	Phase = std::make_unique<Sequencer>();
	Phase->addChild(*movePhase);
	Phase->addChild(*attackPhase);

	addChild(*Phase);
	enemy->loadYamlFile();
	moveVelRotaQuaternion = XMQuaternionRotationRollPitchYaw(0, XM_PIDIV2, 0);
	
	enemy->moveSpeed = BaseActObj::moveSpeedDef * 10.f;
}


std::optional<EnemyData> BaseEnemy::loadYamlFile()
{
	constexpr const char filePath[] = "Resources/DataFile/enemy.yml";

	std::string data{};
	{
		std::ifstream ifs(filePath);
		if (!ifs) { return std::nullopt; }

		std::string line{};
		while (std::getline(ifs, line))
		{
			data += line + "\n";
		}
		ifs.close();
	}

	Yaml::Node root{};
	
	auto& Enemy_1 = root["Enemy_1"];
	Enemy_1["speed"].As<float>();
	enemy1Data.moveSpeed = Enemy_1["speed"].As<float>();
	enemy1Data.normalSpeed = enemy1Data.moveSpeed;
	enemy1Data.dashSpeed = enemy1Data.normalSpeed * Enemy_1["dashSpeedRate"].As<float>();
	enemy1Data.dashSpeedAttenuation = enemy1Data.normalSpeed * Enemy_1["dashSpeedAttRate"].As<float>();
	enemy1Data.hp = Enemy_1["hp"].As<uint16_t>();
	enemy1Data.attack = Enemy_1["attack"].As<uint16_t>();

	auto& Enemy_2 = root["Enemy_2"];
	Enemy_2["speed"].As<float>();
	enemy2Data.moveSpeed = Enemy_2["speed"].As<float>();
	enemy2Data.normalSpeed = enemy2Data.moveSpeed;
	enemy2Data.dashSpeed = enemy2Data.normalSpeed * Enemy_2["dashSpeedRate"].As<float>();
	enemy2Data.dashSpeedAttenuation = enemy2Data.normalSpeed * Enemy_2["dashSpeedAttRate"].As<float>();
	enemy2Data.hp = Enemy_2["hp"].As<uint16_t>();
	enemy2Data.attack = Enemy_2["attack"].As<uint16_t>();

	auto& Enemy_Boss = root["Enemy_Boss"];
	Enemy_Boss["speed"].As<float>();
	bossData.moveSpeed = Enemy_Boss["speed"].As<float>();
	bossData.normalSpeed = bossData.moveSpeed;
	bossData.dashSpeed = bossData.normalSpeed * Enemy_Boss["dashSpeedRate"].As<float>();
	bossData.dashSpeedAttenuation = bossData.normalSpeed * Enemy_Boss["dashSpeedAttRate"].As<float>();
	bossData.hp = Enemy_Boss["hp"].As<uint16_t>();
	bossData.attack = Enemy_Boss["attack"].As<uint16_t>();


	return std::optional<EnemyData>(enemy1Data);
}


EnemyBehavior::EnemyBehavior() :
	EnemyBehavior(nullptr)
{}

void EnemyBehavior::drawImGui()
{}

NODE_RESULT EnemyBehavior::Phase_move()
{
	if (preBeatCount != enemy->nowBeatCount)
	{
		enemy->MoveProcess(moveVel);
		if (++moveCount > moveCountMax)
		{
			moveVel = XMVector3Rotate(moveVel, moveVelRotaQuaternion);

			moveCount = 0ui16;
			return NODE_RESULT::SUCCESS;
		}
	}

	return NODE_RESULT::RUNNING;
}

NODE_RESULT EnemyBehavior::Phase_Attack()
{
	if (preBeatCount != enemy->nowBeatCount)
	{
		enemy->attackFlag = true;
		enemy->setCol(XMFLOAT4(0.5f, 1, 1, 1));
		if (++attackCount > attackCountMax)
		{
			enemy->attackFlag = false;
			attackCount = 0ui16;
			enemy->setCol(XMFLOAT4(1, 1, 1, 1));
			return NODE_RESULT::SUCCESS;
		}
	}

	return NODE_RESULT::RUNNING;
}

