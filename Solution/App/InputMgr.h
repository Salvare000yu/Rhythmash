#pragma once
#include <Input/Input.h>

enum class ACTION
{
	WEEKATTACK,
	HARDATTACK,
	JUMP,
	STEP,

	MOVE = 100,
	CAMERA
};

class InputMgr
{
private:
	Input* input = nullptr;
public:
	InputMgr();
	bool GetInput(ACTION act);
	DirectX::XMFLOAT2 GetThumbValue(ACTION act);

};