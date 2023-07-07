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
	float rate;				//マウスの感度
public:
	inline static InputMgr* getInstance()
	{
		static InputMgr inputMgr{};
		return &inputMgr;
	}
	inline static InputMgr* ins()
	{
		return getInstance();
	}

	InputMgr();
	bool GetInput(ACTION act);
	DirectX::XMFLOAT2 GetThumbValue(ACTION act);
};