#include "Player.h"
#include "Enemy/BaseEnemy.h"
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
	if (input->triggerKey(DIK_SPACE))
	{
		if (AttackFlag == false)
		{
			AttackFlag = true;
		}
		this->setCol({ 0,0,1,1 });
	}
	if (AttackFlag == true)
	{
		WaitTime++;
		if (WaitTime >= 2)
		{
			AttackFlag = false;
			AtkObj->setCol({ 1,1,1,1 });
			WaitTime = 0;
		}
	}
	//AttackProcess();
}
