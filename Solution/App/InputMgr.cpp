#include "InputMgr.h"

InputMgr::InputMgr()
{
	input = Input::getInstance();
}

bool InputMgr::GetInput(ACTION act)
{
	bool inp = false;
	switch (act)
	{
	case ACTION::WEEKATTACK:
		if (input->triggerMouseButton(Input::PAD::X) && input->triggerMouseButton(Input::MOUSE::LEFT))inp = true;
		break;
	case ACTION::HARDATTACK:
		if (input->triggerMouseButton(Input::PAD::Y) && input->triggerMouseButton(Input::MOUSE::RIGHT))inp = true;
		break;
	case ACTION::JUMP:
		if (input->triggerMouseButton(Input::PAD::A) && input->triggerKey(DIK_SPACE))inp = true;
		break;
	case ACTION::STEP:
		if (input->triggerMouseButton(Input::PAD::RB) && input->triggerKey(DIK_LSHIFT))inp = true;
		break;
	default:
		inp = false;
		break;
	}
	return inp;
}

DirectX::XMFLOAT2 InputMgr::GetThumbValue(ACTION act)
{
	DirectX::XMFLOAT2 inp = {};
	float max = 1.0f;
	switch (act)
	{
	case ACTION::MOVE:
		inp = input->hitPadLStickRaito();

		if (input->hitKey(DIK_W))inp.y = max;
		if (input->hitKey(DIK_S))inp.y = -max;
		if (input->hitKey(DIK_D))inp.x = max;
		if (input->hitKey(DIK_A))inp.x = -max;
		break;
	case ACTION::CAMERA:
		inp = input->hitPadRStickRaito();

		break;
	default:
		break;
	}

	return inp;
}
