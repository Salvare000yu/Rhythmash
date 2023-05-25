#pragma once
#include "System/GameScene.h"
#include <memory>
#include <DirectXMath.h>

class Input;
class Player;
class BaseEnemy;
class ObjModel;
class Object3d;
class GameObj;
class CameraObj;
class SpriteBase;
class Sprite;
class Light;
class Timer;

class GameMainScene :
	public GameScene
{
	// シングルトンインスタンス格納変数
	Input* input = nullptr;

	// --------------------
	// 時間関係
	// --------------------
	std::unique_ptr<Timer> timer;

	float bpm{};
	const float judgeOkRange{};
	float nowCount{};
	float nowBeatRaito{};

	// --------------------
	// スプライト
	// --------------------
	std::unique_ptr<SpriteBase> spCom;

	// --------------------
	// 背景
	// --------------------
	std::unique_ptr<ObjModel> groundModel;
	std::unique_ptr<Object3d> groundObj;

	// --------------------
	// ゲームオブジェクト
	// --------------------

	//プレイヤーの生成
	std::unique_ptr<Player> player;
	std::unique_ptr<ObjModel> playerModel;
	std::unique_ptr<GameObj> playerObj;
	// --------------------
	std::unique_ptr<BaseEnemy> enemy;
	std::unique_ptr<ObjModel> enemyModel;
	std::unique_ptr<GameObj> enemyObj;
	// --------------------
	std::unique_ptr<CameraObj> cameraObj;
	std::unique_ptr<Light> light;
	// --------------------
public:
	GameMainScene();
	void start() override;
	void update() override;
	void drawFrontSprite() override;
};

