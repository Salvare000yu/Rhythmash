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

DirectX::XMFLOAT2 InputMgr::calcMoveValue(InputMgr::MOVE_INPUT act)
{
	DirectX::XMFLOAT2 inp = DirectX::XMFLOAT2(0.f, 0.f);
	constexpr float inputMax = 1.0f;

	switch (act)
	{
	case MOVE_INPUT::PLAYER:
		getMovePlayerInputValue(inp);
		break;
	case MOVE_INPUT::CAMERA:
		getMoveCameraInputValue(inp);
		break;
	default:
		break;
	}

	return inp;
}

void InputMgr::getMovePlayerInputValue(DirectX::XMFLOAT2& outBuf)
{
	// スティックの入力があったかどうか
	const bool inputLFlag =
		input->isVaildPadLStickX() ||
		input->isVaildPadLStickY();

	//コントローラー入力を得る（パッド優先）
	if (inputLFlag)
	{
		outBuf = input->hitPadLStickRaito();
		return;
	}

	constexpr float inputMax = 1.f;
	if (input->hitKey(DIK_W)) { outBuf.y = inputMax; }
	if (input->hitKey(DIK_S)) { outBuf.y = -inputMax; }
	if (input->hitKey(DIK_D)) { outBuf.x = inputMax; }
	if (input->hitKey(DIK_A)) { outBuf.x = -inputMax; }
}

void InputMgr::getMoveCameraInputValue(DirectX::XMFLOAT2& outBuf)
{
	// スティックの入力があったかどうか
	const bool inputRFlag =
		input->isVaildPadRStickX() ||
		input->isVaildPadRStickY();

	// コントローラー入力を得る（パッド優先）
	if (inputRFlag)
	{
		outBuf = input->hitPadRStickRaito();
		return;
	}

	outBuf.x = static_cast<float>(input->getMouseMove().x);
	outBuf.y = static_cast<float>(input->getMouseMove().y);
	outBuf.x /= mouseRate;
	outBuf.y /= mouseRate;
}
