#pragma once
#include "System/GameScene.h"
#include <memory>
#include <DirectXMath.h>
#include <vector>

class Input;
class Player;
class BaseEnemy;
class ObjModel;
class Object3d;
class GameObj;
class CameraObj;
class Light;
class Timer;
class SpriteBase;
class Sprite;
class Util;
class Sound;

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
	// エネミー関連--------------------
	std::vector<std::shared_ptr<BaseEnemy>> enemy;
	std::unique_ptr<ObjModel> enemyModel;
	std::unique_ptr<GameObj> enemyObj;


	std::unique_ptr<CameraObj> cameraObj;
	std::unique_ptr<Light> light;

	std::unique_ptr<GameObj> stageObj;
	std::unique_ptr<ObjModel> stageModel;

	std::unique_ptr<Sound> sound;
	std::shared_ptr<Sound> damageSe;
public:
	GameMainScene();
	void start() override;
	void update() override;
	void drawObj3d() override;
	void drawFrontSprite() override;

	std::weak_ptr<BaseEnemy> addEnemy(const DirectX::XMFLOAT3& pos);
};
