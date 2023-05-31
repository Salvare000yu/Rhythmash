#include "Player.h"
#include "Enemy/BaseEnemy.h"
#include <InputMgr.h>
#include <cmath>
#include <Util/Timer.h>

#include <Input/Input.h>

using namespace DirectX;

Player::Player(Camera* camera,
			   ObjModel* model,
			   const DirectX::XMFLOAT3& pos) :
	BaseActObj(camera, model, pos),
	input(Input::ins())
{
	se1 = std::make_unique<Sound>("Resources/SE/Sys_Set03-click.wav");

	moveSpeed = 20.f;

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

	// 該当キーが押されていれば移動する
	const bool moved = hitW || hitA || hitS || hitD;

	// 移動しなければ終了
	if (!moved) { return; }

	// 移動方向を決める
	XMFLOAT3 dir{};
	if (hitW)
	{
		dir.z = 1.f;
	} else if (hitS)
	{
		dir.z = -1.f;
	}

	if (hitD)
	{
		dir.x = 1.f;
	} else if (hitA)
	{
		dir.x = -1.f;
	}

	// 移動する
	MoveProcess(dir);
}

void Player::Attack()
{
	if (input->triggerKey(DIK_SPACE) && judgeRet)
	{
		Sound::SoundPlayWave(se1.get());
		attackFlag = true;
		this->setCol({ 0,0,1,1 });
	}

	AttackProcess();

	if (!attackFlag)
	{
		this->setCol({ 1,1,1,1 });
	}
}

void Player::Step()
{
	constexpr float defSpeed = BaseActObj::moveSpeedDef;
	constexpr float dashSpeed = defSpeed * 3.f;
	constexpr float speedAcc = -dashSpeed / 12.f;

	if (Input::ins()->triggerKey(DIK_LSHIFT) && judgeRet)
	{
		SetSpeed(dashSpeed);
	} else
	{
		SetSpeed(std::max(defSpeed, GetSpeed() + speedAcc));
	}
}