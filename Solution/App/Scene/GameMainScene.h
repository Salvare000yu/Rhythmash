#pragma once
#include "System/GameScene.h"
#include <memory>
#include "2D/DebugText.h"
#include "Input/Input.h"
#include "Player/Player.h"
class GameMainScene :
	public GameScene
{
	// --------------------
	// スプライト
	// --------------------
	std::unique_ptr<SpriteBase> spCom;
	//std::unique_ptr<Sprite> clear;

	// --------------------
	// デバッグテキスト
	// --------------------
	std::unique_ptr<DebugText> debugText;

	Input* input = nullptr;
	//プレイヤーの生成
	std::unique_ptr<Player> player;
public:
	GameMainScene();
	void start() override;
	void update() override;
	void drawFrontSprite() override;
};

