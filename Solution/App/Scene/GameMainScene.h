#pragma once
#include "System/GameScene.h"
#include <CollisionMgr.h>
#include <System/PostEffect.h>
#include <Util/Timer.h>
#include <Enemy/EnemyMgr.h>
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
	std::shared_ptr<Timer> timer;

	const float judgeOkRange{};
	float nowCount{};
	float nowBeatRaito{};

	// --------------------
	// スプライト
	// --------------------
	std::unique_ptr<SpriteBase> spriteBase;
	std::unique_ptr<Sprite> rhythmUi;
	std::unique_ptr<Sprite> judgeRangeSprite;

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

	// 敵
	std::unique_ptr<EnemyMgr> enemyMgr;
	std::unordered_map<std::string, std::unique_ptr<ObjModel>> enemyModels;
	CollisionMgr::ColliderSet enemyCols;

	std::unique_ptr<CameraObj> cameraObj;
	std::unique_ptr<Light> light;

	std::unique_ptr<GameObj> stageObj;
	std::unique_ptr<GameObj> stageOtherObj;
	std::unique_ptr<ObjModel> stageModel;
	std::unique_ptr<ObjModel> stageOtherModel;

	std::weak_ptr<SoundData> bgm;
	std::weak_ptr<SoundData> damageSe;

	struct EnemyFileDataFormat
	{
		uint16_t hp = 5ui16;
		uint16_t attack = 1ui16;
		float speed = 10.f;

		ObjModel* model = nullptr;
		float scale = 1.f;

		std::string behaviorStr{};
	};
	std::unordered_map<std::string, EnemyFileDataFormat> loadedData;
	struct WaveDataEnemyPos
	{
		std::string tag{};
		std::forward_list<DirectX::XMFLOAT3> pos{};
	};
	struct WaveData
	{
		std::forward_list<WaveDataEnemyPos> data{};
	};
	std::list<WaveData> waveData{};
	std::list<WaveData>::const_iterator nextWaveIt;

private:
	void initPostEffect();
	void initLight();
	void initCamera();
	void initPlayer();
	void initParticle();
	void initBack();
	void initEnemy();
	bool loadEnemyFile();
	void initCollider();
	void initSound();
	void initSprite();

	// RGBずらし
	class RgbShiftData
	{
		bool activeFlag;
		std::unique_ptr<Timer> timer;

	public:
		RgbShiftData() : activeFlag(false), timer(std::make_unique<Timer>()) {}

		inline Timer::timeType getNowTime() const { return this->timer->getNowTime(); }

		static constexpr Timer::timeType timeMax = Timer::oneSec / 2;

		inline void reset()
		{
			PostEffect::ins()->setRgbShiftNum(DirectX::XMFLOAT2(0.f, 0.f));
			activeFlag = false;
			timer->reset();
		}

		inline bool isActive() const { return activeFlag; }

		inline void start()
		{
			activeFlag = true;
			timer->reset();
		}

		void update();
	};
	RgbShiftData rgbShiftData{};

	void updateCollision();
	void updateBeatData();

	void updateLight();

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
	std::weak_ptr<BaseEnemy> addEnemy(const DirectX::XMFLOAT3& pos, const EnemyMgr::EnemyParam& enemyParam, ObjModel* model, float scale, const std::string& behaviorStr);

	void startWave(const std::list<WaveData>::const_iterator& wave);

	/// @brief 自機の移動処理
	void movePlayer();
};
