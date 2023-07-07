#include "Player.h"
#include "Enemy/BaseEnemy.h"
#include <InputMgr.h>
#include <Camera/CameraObj.h>
#include <cmath>
#include <Util/Timer.h>

#include <Input/Input.h>

#include <fstream>
#include <Yaml.hpp>

using namespace DirectX;

bool Player::loadYamlFile()
{
	constexpr const char filePath[] = "Resources/DataFile/player.yml";

	std::string data{};
	{
		std::ifstream ifs(filePath);
		if (!ifs) { return true; }

		std::string line{};
		while (std::getline(ifs, line))
		{
			data += line + "\n";
		}
		ifs.close();
	}

	Yaml::Node root{};
	try
	{
		Yaml::Parse(root, data);
	} catch (...)
	{
		return true;
	}

	auto& pNode = root["playerData"];
	moveSpeed = pNode["speed"].As<float>();
	normalSpeed = moveSpeed;
	dashSpeed = normalSpeed * pNode["dashSpeedRate"].As<float>();
	dashSpeedAttenuation = -normalSpeed * pNode["dashSpeedAttRate"].As<float>();
	setHp(pNode["hp"].As<uint16_t>());
	setAttack(pNode["attack"].As<uint16_t>());
	invincibleFrameMax = pNode["invincible"].As<uint32_t>();

	auto& posNode = pNode["position"];
	setPos(XMFLOAT3(
		posNode["x"].As<float>(),
		posNode["y"].As<float>(),
		posNode["z"].As<float>()
	));

	return false;
}

Player::Player(Camera* camera,
			   ObjModel* model,
			   const DirectX::XMFLOAT3& pos) :
	BaseActObj(camera, model, pos)
{
	se1 = Sound::ins()->loadWave("Resources/SE/Sys_Set03-click.wav");

	// データをYAMLファイルから読み込む
	loadYamlFile();

	// ダメージを受けたら無敵状態になる
	addDamageProc([&] { invincibleFrag = true; });

	// 更新処理を設定
	update_proc =
		[&]
	{
		if (!this->getAlive())
		{
			update_proc = [] {};
			this->setCol({ 0,0,0,getCol().w });
			return;
		}

		attack();
		step();
		invincible();
	};

	// 更新処理を登録
	additionalUpdateProc.emplace("Player::update_proc", [&] { update_proc(); });
}

void Player::attack()
{
	// タイミングよく入力で攻撃
	if (Input::ins()->triggerKey(DIK_SPACE) && judge())
	{
		Sound::playWave(se1);
		attackFlag = true;
		this->setCol(XMFLOAT4(0, 0, 1, getCol().w));

		if (!atkObjPt.expired())
		{
			auto atkObj = atkObjPt.lock();
			atkObj->setCol(XMFLOAT4(0, 1, 0, atkObj->getCol().w));
		}
	}

	// フレーム更新処理
	if (attackFlag)
	{
		if (++attackFrame >= 2ui32)
		{
			attackFlag = false;
			attackFrame = 0;
		}
	}
}

void Player::step()
{
	if (Input::ins()->triggerKey(DIK_C) && judge())
	{
		setSpeed(dashSpeed);
	} else
	{
		const float acc = normalSpeed * dashSpeedAttenuation;
		setSpeed(std::max(normalSpeed, moveSpeed + acc));
	}
}

void Player::invincible()
{
	if (!invincibleFrag) { return; }
	if (++invincibleFrame > invincibleFrameMax)
	{
		invincibleFrame = 0;
		invincibleFrag = false;
	}
}