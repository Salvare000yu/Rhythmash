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
	moveSpeed = 20.f;

	update_proc =
		[&]
	{
		Move();
		Attack();

		if (!this->getAlive())
		{
			update_proc = [] {};
			this->setCol({ 0,0,0,1 });
		}

		Step();
	};
}

void Player::additionalUpdate()
{
	update_proc();
}

void Player::Move()
{
	bool moved = false;
	XMFLOAT3 dir = { 0,0,0 };

	if (input->hitKey(DIK_W))
	{
		dir.z = 1.f;
		moved = true;
	} else if (input->hitKey(DIK_S))
	{
		dir.z = -1.f;
		moved = true;
	}

	if (input->hitKey(DIK_D))
	{
		dir.x = 1.f;
		moved = true;
	} else if (input->hitKey(DIK_A))
	{
		dir.x = -1.f;
		moved = true;
	}

	if (moved) { MoveProcess(dir); }
}

void Player::Attack()
{
	if (input->hitKey(DIK_SPACE))
	{
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
	float stepRange = 45.0f;
	float subRate = 5.0f;
	if (input->triggerKey(DIK_LSHIFT) && judgeRet)
	{
		SetSpeed(stepRange);	
	}
	float speed = GetSpeed();
	if (speed > MOVE_SPEED)
	{
		speed -= subRate;
		speed = std::max(speed, MOVE_SPEED);
	}
	SetSpeed(speed);
}
