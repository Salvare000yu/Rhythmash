#include "Player.h"

Player::Player(Camera* camera,
						 ObjModel* model,
						 const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera,model, pos)
{
	
	input = Input::getInstance();
}

void Player::start()
{}

void Player::update()
{
	//{
	//	if (input->hitKey(DIK_W))
	//	{
	//		PlayerObj->moveForward(val);
	//		moveZFlag = true;
	//	} else if (input->hitKey(DIK_S))
	//	{
	//		PlayerObj->moveForward(-val);
	//		moveZFlag = true;
	//	} else
	//	{
	//		moveZFlag = false;
	//	}
	//	if (input->hitKey(DIK_D))
	//	{
	//		PlayerObj->moveRight(val);
	//		moveXFlag = true;
	//	} else if (input->hitKey(DIK_A))
	//	{
	//		PlayerObj->moveRight(-val);
	//		moveXFlag = true;
	//	} else
	//	{
	//		moveXFlag = false;
	//	}
	//	float mag = (val * val + val * val);
	//	if (moveZFlag == true && moveXFlag == true)
	//	{

	//		val = val / mag;
	//	}

	//}
	
	
	Move();

	
}

void Player::Draw()
{
	
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
{}

void Player::Input()
{
	//ジャンプ
	if (input->triggerPadButton(XINPUT_GAMEPAD_A) && !isJump)isJump = true;
	//回避
	if (input->triggerMouseButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) && !isStep)isStep = true;
	//弱攻撃
	if (input->triggerMouseButton(XINPUT_GAMEPAD_X) && !isWeekAttack)isWeekAttack = true;
	//強攻撃
	if (input->triggerMouseButton(XINPUT_GAMEPAD_Y) && !isHardAttack)isHardAttack = true;
}
