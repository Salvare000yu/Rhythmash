#include "Player.h"
#include "Enemy/BaseEnemy.h"
#include <InputMgr.h>
#include <cmath>

Player::Player(Camera* camera,
						 ObjModel* model,
						 const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos)
{
	input = Input::getInstance();
}

void Player::update()
{
	Move();

	Attack();
	if (!this->getAlive())
	{
		this->setCol({ 0,0,0,1 });
	}

	Step();
	
}

void Player::Move()
{
	dir = { 0,0,0 };

	if (input->hitKey(DIK_W))
	{
		dir.z = 5;
		MoveProcess(dir);
	} else if (input->hitKey(DIK_S))
	{
		dir.z = -5;
		MoveProcess(dir);
	}

	if (input->hitKey(DIK_D))
	{
		dir.x = 5;
		MoveProcess(dir);
	} else if (input->hitKey(DIK_A))
	{
		dir.x = -5;
		MoveProcess(dir);
	}
}

void Player::Attack()
{
	if (input->hitKey(DIK_SPACE))
	{
		AttackFlag = true;
		this->setCol({ 0,0,1,1 });
	}

	AttackProcess();
}

void Player::Step()
{
	float stepRange = 5.0f;
	float subRate = 0.5f;
	if (input->triggerKey(DIK_LSHIFT))
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
