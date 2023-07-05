#include "BaseEnemy.h"
#include <Util/RandomNum.h>
#include <BehaviorTree/Sequencer.h>
#include <Util/RandomNum.h>

#include <fstream>
#include <Yaml.hpp>
using namespace DirectX;

BaseEnemy::BaseEnemy(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos),
	enemyBehavior(std::make_unique<EnemyBehavior>(this))
{
	loadEnemyData();
	additionalUpdateProc.emplace("BaseEnemy::update_proc", [&] { enemyBehavior->run(); });
	additionalUpdateProc.emplace("BaseEnemy::カウント更新", [&] { enemyBehavior->setPreBeatCount(nowBeatCount); });
}

void BaseEnemy::loadEnemyData()
{
	std::string fullpath = "Resources/DataFile/enemy.yml";

	std::string Data;
	std::ifstream file(fullpath);
	if (file.fail())
	{
		return;
	}
	std::string line{};
	while (std::getline(file, line))
	{
		Data += line + "\n";
	}

	Yaml::Node node{};
	try
	{
		Yaml::Parse(node, Data);
		auto& Enemy_1 = node["Enemy_1"];
		enemy1Data.moveSpeed = Enemy_1["speed"].As<float>();
		enemy1Data.dashSpeed = Enemy_1["dashSpeedRate"].As<float>();
		enemy1Data.dashSpeedAttenuation = Enemy_1["dashSpeedAttRate"].As<float>();
		enemy1Data.hp = Enemy_1["hp"].As<uint16_t>();
		enemy1Data.attack = Enemy_1["attack"].As<uint16_t>();

		auto& Enemy_2 = node["Enemy_2"];
		enemy2Data.moveSpeed = Enemy_2["speed"].As<float>();
		enemy2Data.dashSpeed = Enemy_2["dashSpeedRate"].As<float>();
		enemy2Data.dashSpeedAttenuation = Enemy_2["dashSpeedAttRate"].As<float>();
		enemy2Data.hp = Enemy_2["hp"].As<uint16_t>();
		enemy2Data.attack = Enemy_2["attack"].As<uint16_t>();

		auto& Enemy_Boss = node["Enemy_Boss"];
		bossData.moveSpeed = Enemy_Boss["speed"].As<float>();
		bossData.dashSpeed = Enemy_Boss["dashSpeedRate"].As<float>();
		bossData.dashSpeedAttenuation = Enemy_Boss["dashSpeedAttRate"].As<float>();
		bossData.hp = Enemy_Boss["hp"].As<uint16_t>();
		bossData.attack = Enemy_Boss["attack"].As<uint16_t>();
	} catch (const Yaml::Exception)
	{
		return;
	}

}


float BaseEnemy::TargetFromDistance()
{
	if (!this->targetObj) { return -1.f; }

	const XMVECTOR pos = XMLoadFloat3(&this->calcWorldPos());
	const XMVECTOR tpos = XMLoadFloat3(&this->targetObj->calcWorldPos());

	return Collision::vecLength(pos - tpos);
}

void BaseEnemy::drawIdmGui()
{
	enemyBehavior->drawImGui();
}
