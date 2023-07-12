#pragma once

#include <DirectXMath.h>

class Input;

/// @brief 入力を管理するクラス
class InputMgr
{
public:
	enum class PLAYER_ACTION_INPUT : uint8_t
	{
		WEEKATTACK,
		HARDATTACK,
		JUMP,
		STEP,
	};

	enum class MOVE_INPUT : uint8_t
	{
		PLAYER,
		CAMERA,
	};

private:
	Input* input = nullptr;

	/// @brief マウスの感度
	float mouseRate;

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
	bool getInput(InputMgr::PLAYER_ACTION_INPUT act);
	bool calcMoveValue(InputMgr::MOVE_INPUT act, DirectX::XMFLOAT2& outBuf);

private:
	bool getMovePlayerInputValue(DirectX::XMFLOAT2& outBuf);
	bool getMoveCameraInputValue(DirectX::XMFLOAT2& outBuf);
};