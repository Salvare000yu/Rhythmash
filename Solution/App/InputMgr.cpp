#include "InputMgr.h"

InputMgr::InputMgr()
{
	input = Input::getInstance();
	rate = 10.0f;
}

bool InputMgr::GetInput(ACTION act)
{
	bool inp = false;
	//それぞれのアクションの入力判定を取る
	switch (act)
	{
	case ACTION::WEEKATTACK:
		if (input->triggerPadButton(Input::PAD::X) || input->triggerMouseButton(Input::MOUSE::LEFT))inp = true;
		break;
	case ACTION::HARDATTACK:
		if (input->triggerPadButton(Input::PAD::Y) || input->triggerMouseButton(Input::MOUSE::RIGHT))inp = true;
		break;
	case ACTION::JUMP:
		if (input->triggerPadButton(Input::PAD::A) || input->triggerKey(DIK_SPACE))inp = true;
		break;
	case ACTION::STEP:
		if (input->triggerPadButton(Input::PAD::RB) || input->triggerKey(DIK_C))inp = true;
		break;
	default:
		inp = false;
		break;
	}
	return inp;
}

DirectX::XMFLOAT2 InputMgr::GetThumbValue(ACTION act)
{
	DirectX::XMFLOAT2 inp = DirectX::XMFLOAT2(0.0f, 0.0f);
	float max = 1.0f;
	switch (act)
	{
	case ACTION::MOVE:
		//コントローラー入力を得る
		inp = input->hitPadLStickRaito();

		//コントローラが入力されていなかったらキー入力を得る
		if (inp.x == 0.0f && inp.y == 0.0f)
		{
			if (input->hitKey(DIK_W))inp.y = max;
			if (input->hitKey(DIK_S))inp.y = -max;
			if (input->hitKey(DIK_D))inp.x = max;
			if (input->hitKey(DIK_A))inp.x = -max;
		}
		break;
	case ACTION::CAMERA:
		//コントローラー入力を得る
		inp = input->hitPadRStickRaito();

		//コントローラーが入力されていなかったらマウス移動量を得る
		if (inp.x == 0.0f && inp.y == 0.0f)
		{
			inp.x = static_cast<float>(input->getMouseMove().x);
			inp.y = static_cast<float>(input->getMouseMove().y);
			inp.x /= rate;
			inp.y /= rate;
		}

		break;
	default:
		break;
	}

	return inp;
}