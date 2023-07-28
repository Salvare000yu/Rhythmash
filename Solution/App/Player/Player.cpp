#include "Player.h"
#include "Enemy/BaseEnemy.h"
#include <InputMgr.h>
#include <Camera/CameraObj.h>
#include <cmath>
#include <Util/Timer.h>
#include <Input/Input.h>
#include <3D/ParticleMgr.h>
#include <Util/Timer.h>

#include <fstream>
#include <Yaml.hpp>

using namespace DirectX;

namespace
{
	constexpr uint32_t attackTimeMax = Timer::oneSec / 2ui32;
}

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
			   std::weak_ptr<Timer> timer,
			   const DirectX::XMFLOAT3& pos) :
	BaseActObj(camera, model, timer, pos),
	attackTimer(std::make_unique<Timer>())
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
	additionalUpdateProc.emplace("Player::update_proc", update_proc);

	// 攻撃当たり判定用オブジェクトの設定
	attackModel = std::make_unique<ObjModel>("Resources/sphere/", "sphere", 0U, true);

	attackObj = std::make_unique<GameObj>(camera, attackModel.get());;
	attackObj->setParent(this->getObj());
	attackObj->setScale(6.f);
	attackObj->setCol(XMFLOAT4(0.5f, 1, 1, 0.25f));

	attackObjCol = CollisionMgr::ColliderType::create(attackObj.get(), attackObj->getScaleF3().z);
	attackObjColSet.group.emplace_front(attackObjCol);

	additionalUpdateProc.emplace("Player::update_attackObj", [&] { attackObj->update(); });
	additionalDrawProc.emplace("Player::draw_attackObj", [&](Light* light) { if (attackFlag) { attackObj->draw(light); } });

	// 最初は無敵でスタート
	invincibleFrag = true;
}

void Player::attack()
{
	// タイミングよく入力で攻撃
	if (Input::ins()->triggerKey(DIK_SPACE) && judge())
	{
		startAttack();
	}

	// フレーム更新処理
	if (!attackFlag) { return; }

	// 指定時間経過したら攻撃状態終了
	if (attackTimer->getNowTime() >= attackTimeMax)
	{
		attackFlag = false;
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

void Player::startAttack()
{
	Sound::playWave(se1);
	attackFlag = true;
	attackTimer->reset();
}
