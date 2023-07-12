#include "InputMgr.h"
#include <Input/Input.h>

InputMgr::InputMgr() :
	input(Input::ins()),
	mouseRate(10.f)
{}

bool InputMgr::getInput(PLAYER_ACTION_INPUT act)
{
	// それぞれのアクションの入力判定を取る
	switch (act)
	{
	case PLAYER_ACTION_INPUT::WEEKATTACK:
		if (input->triggerPadButton(Input::PAD::X) || input->triggerMouseButton(Input::MOUSE::LEFT)) { return true; }
		break;
	case PLAYER_ACTION_INPUT::HARDATTACK:
		if (input->triggerPadButton(Input::PAD::Y) || input->triggerMouseButton(Input::MOUSE::RIGHT)) { return true; }
		break;
	case PLAYER_ACTION_INPUT::JUMP:
		if (input->triggerPadButton(Input::PAD::A) || input->triggerKey(DIK_SPACE)) { return true; }
		break;
	case PLAYER_ACTION_INPUT::STEP:
		if (input->triggerPadButton(Input::PAD::RB) || input->triggerKey(DIK_C)) { return true; }
		break;
	}
	return false;
}

bool InputMgr::calcMoveValue(InputMgr::MOVE_INPUT act, DirectX::XMFLOAT2& outBuf)
{
	switch (act)
	{
	case MOVE_INPUT::PLAYER:
		return getMovePlayerInputValue(outBuf);
		break;
	case MOVE_INPUT::CAMERA:
		return getMoveCameraInputValue(outBuf);
		break;
	default:
		break;
	}

	return false;
}

bool InputMgr::getMovePlayerInputValue(DirectX::XMFLOAT2& outBuf)
{
	// スティックの入力があったかどうか
	const bool inputLFlag =
		input->isVaildPadLStickX() ||
		input->isVaildPadLStickY();

	//コントローラー入力を得る（パッド優先）
	if (inputLFlag)
	{
		outBuf = input->hitPadLStickRaito();
		return true;
	}

	bool ret = false;

	constexpr float inputMax = 1.f;
	if (input->hitKey(DIK_W)) { outBuf.y = inputMax; ret = true; }
	if (input->hitKey(DIK_S)) { outBuf.y = -inputMax; ret = true; }
	if (input->hitKey(DIK_D)) { outBuf.x = inputMax; ret = true; }
	if (input->hitKey(DIK_A)) { outBuf.x = -inputMax; ret = true; }

	return ret;
}

bool InputMgr::getMoveCameraInputValue(DirectX::XMFLOAT2& outBuf)
{
	// スティックの入力があったかどうか
	const bool inputRFlag =
		input->isVaildPadRStickX() ||
		input->isVaildPadRStickY();

	// コントローラー入力を得る（パッド優先）
	if (inputRFlag)
	{
		outBuf = input->hitPadRStickRaito();
		return true;
	}

	// マウスが動いていなければ動いていない
	if (input->getMouseMove().x == 0 &&
		input->getMouseMove().y == 0)
	{
		return false;
	}

	outBuf.x = static_cast<float>(input->getMouseMove().x);
	outBuf.y = static_cast<float>(input->getMouseMove().y);
	outBuf.x /= mouseRate;
	outBuf.y /= mouseRate;

	return true;
}
