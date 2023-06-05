﻿#pragma once
#include "System/GameScene.h"
#include <memory>
#include <functional>
#include <thread>
#include <DirectXMath.h>
#include "2D/DebugText.h"
#include "Input/Input.h"
// -----------------------
#include "Player/Player.h"
// -----------------------
#include "Enemy/BaseEnemy.h"
#include "3D/Obj/ObjModel.h"
#include "GameObject/GameObj.h"
#include "Camera/CameraObj.h"
#include "Util/Util.h"
#include"Sound/Sound.h"
class GameMainScene :
	public GameScene
{
	// --------------------
	// スプライト
	// --------------------
	std::unique_ptr<SpriteBase> spCom;
	//std::unique_ptr<Sprite> clear;
	std::unique_ptr<Sprite> titleBack;
	std::unique_ptr<Sprite> hpBar;
	// --------------------
	// デバッグテキスト
	// --------------------
	std::unique_ptr<DebugText> debugText;

	Input* input = nullptr;
	//プレイヤーの生成
	std::unique_ptr<Player> player;
	std::unique_ptr<ObjModel> PlayerModel;
	std::unique_ptr<GameObj> PlayerObj;
	// --------------------
	std::unique_ptr<BaseEnemy> enemy;
	std::unique_ptr<ObjModel> EnemyModel;
	std::unique_ptr<GameObj> EnemyObj;
	// --------------------
	std::unique_ptr<CameraObj> cameraobj;
	std::unique_ptr<Light> light;
	// --------------------
	std::unique_ptr<GameObj> StageObj;
	std::unique_ptr<ObjModel> StageModel;

	DirectX::XMFLOAT3 Playerpos = { 0,0,0 };

	DirectX::XMFLOAT3 Enemypos = { 0,0,0 };

	Sound* sound;
public:
	GameMainScene();
	void start() override;
	void update() override;
	void drawFrontSprite() override;
	float hpbar = -1;
};

