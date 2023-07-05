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
	BaseActObj(camera, model, pos),
	inputMgr(InputMgr::ins())
{
	se1 = std::make_unique<Sound>("Resources/SE/Sys_Set03-click.wav");

	loadYamlFile();

	auto atkObj = atkObjPt.lock();
	//atkObj->setPos({ 0,0,0 });

	update_proc =
		[&]
	{
		if (!this->getAlive())
		{
			update_proc = [] {};
			this->setCol({ 0,0,0,1 });
			return;
		}

		Move();
		Attack();
		Step();
		ViewShift();
		Invincible();
	};

	additionalUpdateProc.emplace("Player::update_proc", [&] { update_proc(); });
}

void Player::Move()
{
	// キー入力を取得
	const bool hitW = Input::ins()->hitKey(DIK_W);
	const bool hitA = Input::ins()->hitKey(DIK_A);
	const bool hitS = Input::ins()->hitKey(DIK_S);
	const bool hitD = Input::ins()->hitKey(DIK_D);
	dir = { 0, 0, 0 };

	// 該当キーが押されていればsound
	const bool moved = hitW || hitA || hitS || hitD;

	// 移動しなければ終了
	if (!moved) { return; }

	// 移動方向を決める
	XMFLOAT3 dir{};
	if (hitW)
	{
		dir.z = 1.f;
	}
	if (hitS)
	{
		dir.z = -1.f;
	}
	if (hitD)
	{
		dir.x = 1.f;
	}
	if (hitA)
	{
		dir.x = -1.f;
	}

	// 移動する
	MoveProcess(dir);
}

void Player::Attack()
{
	auto atkObj = atkObjPt.lock();
	if (InputMgr::ins()->GetInput(ACTION::WEEKATTACK) && judge())
	{
		Sound::SoundPlayWave(se1.get());
		attackFlag = true;
		this->setCol(XMFLOAT4(0, 0, 1, 1));
		
		atkObj->setCol(XMFLOAT4(0, 1, 0, atkObj->getCol().w));
	}

	if (attackFlag)
	{
		if (++AttackFrame >= 2)
		{
			attackFlag = false;
			AttackFrame = 0;
		}
	} else
	{
		this->setCol({ 1,1,1,1 });
	}
}

void Player::Step()
{

	if (InputMgr::ins()->GetInput(ACTION::STEP) && judge())
	{
		SetSpeed(dashSpeed);
	} else
	{
		const float acc = normalSpeed * dashSpeedAttenuation;
		SetSpeed(std::max(normalSpeed, moveSpeed + acc));
	}
}
void Player::ViewShift()
{
	DirectX::XMFLOAT2 inp = InputMgr::ins()->GetThumbValue(ACTION::CAMERA);
	DirectX::XMFLOAT3 relativeRotaDeg = cameraObj->getRelativeRotaDeg();

	relativeRotaDeg.x += inp.y;
	relativeRotaDeg.y += inp.x;
	cameraObj->setRelativeRotaDeg(relativeRotaDeg);
}

void Player::Invincible()
{
	if (!invincibleFrag)return;
	if (++invincibleFrame > 120)
	{
		invincibleFrame = 0;
		invincibleFrag = false;
	}
}
