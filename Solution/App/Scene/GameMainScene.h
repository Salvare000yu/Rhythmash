﻿#pragma once
#include "System/GameScene.h"
#include <CollisionMgr.h>
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
class ParticleMgr;
class SoundData;

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

	// パーティクル
	std::shared_ptr<ParticleMgr> particleMgr;

	// 自機
	std::unique_ptr<Player> player;
	std::unique_ptr<ObjModel> playerModel;
	std::unique_ptr<GameObj> playerObj;
	CollisionMgr::ColliderSet playerCols;
	CollisionMgr::ColliderSet playerAtkCols;

	// 敵
	std::vector<std::shared_ptr<BaseEnemy>> enemy;
	std::unique_ptr<ObjModel> enemyModel;
	std::unique_ptr<GameObj> enemyObj;
	CollisionMgr::ColliderSet enemyCols;
	CollisionMgr::ColliderSet enemyAtkCols;

	std::unique_ptr<CameraObj> cameraObj;
	std::unique_ptr<Light> light;

	std::unique_ptr<GameObj> stageObj;
	std::unique_ptr<ObjModel> stageModel;

	std::weak_ptr<SoundData> bgm;
	std::weak_ptr<SoundData> damageSe;
public:
	GameMainScene();
	~GameMainScene();

	void start() override;
	void update() override;
	void drawObj3d() override;
	void drawFrontSprite() override;

private:
	/// @brief 敵リストに要素を追加
	/// @param pos 追加する敵の位置
	/// @return 追加した敵を示すweak_ptr
	std::weak_ptr<BaseEnemy> addEnemy(const DirectX::XMFLOAT3& pos);

	/// @brief 自機の移動処理
	void movePlayer();
};
