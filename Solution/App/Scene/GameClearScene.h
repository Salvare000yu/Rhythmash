#pragma once
#include "System/GameScene.h"

#include <memory>
#include "2D/DebugText.h"
#include "Input/Input.h"

class Timer;

class GameClearScene :
	public GameScene
{
	std::unique_ptr<Timer> timer;

	// --------------------
	// スプライト
	// --------------------
	std::unique_ptr<SpriteBase> spCom;
	std::unique_ptr<Sprite> clear;

	// --------------------
	// デバッグテキスト
	// --------------------
	std::unique_ptr<DebugText> debugText;

	Input* input = nullptr;

public:
	GameClearScene();
	void start() override;
	void update() override;
	void drawFrontSprite() override;
};
