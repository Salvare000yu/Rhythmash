#include "Player.h"

Player::Player()
{
	PlayerModel.reset(new ObjModel("Resources/cube/", "cube"));
	cameraobj.reset(new CameraObj(PlayerObj.get()));
	PlayerObj.reset(new GameObj(cameraobj.get(), PlayerModel.get(), { 0,0,0 }));
	light.reset(new Light());
}

void Player::start()
{}

void Player::update(Input* input, float val)
{
	
	if (input->hitKey(DIK_W))
	{
		PlayerObj->moveForward(val);
		moveZFlag = true;
	} else if (input->hitKey(DIK_S))
	{
		PlayerObj->moveForward(-val);
		moveZFlag = true;
	} else
	{
		moveZFlag = false;
	}
	if (input->hitKey(DIK_D))
	{
		PlayerObj->moveRight(val);
		moveXFlag = true;
	} else if (input->hitKey(DIK_A))
	{
		PlayerObj->moveRight(-val);
		moveXFlag = true;
	} else
	{
		moveXFlag = false;
	}
	float mag = (val * val + val * val);
	if (moveZFlag == true && moveXFlag == true)
	{
		
		val = val / mag;
	}
	PlayerObj->update();
}

void Player::Draw()
{
	PlayerObj->drawWithUpdate(light.get());
}
