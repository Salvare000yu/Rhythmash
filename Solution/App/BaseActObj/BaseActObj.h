﻿#pragma once
#include <DirectXMath.h>
#include <GameObject/GameObj.h>
#include <CollisionMgr.h>

class Light;
class Camera;
class ParticleMgr;
class SoundData;
class ObjModel;

class BaseActObj
	: public GameObj
{
public:
	static constexpr float moveSpeedDef = 15.f;

private:
	std::weak_ptr<SoundData> damage;

	float nowBeatRaito = 0.f;

	std::weak_ptr<ParticleMgr> particle;

protected:
	std::function<bool()> judge;

	//移動スピード
	float moveSpeed = moveSpeedDef;

	int waitFrame = 0;

	Light* light;

	std::weak_ptr<GameObj> atkObjPt;

	bool attackFlag = false;

	/// @brief 無敵状態かどうか
	bool invincibleFrag;

	/// @brief 無敵状態の最大持続時間
	uint32_t invincibleFrameMax;

	/// @brief 無敵状態の現在の経過時間
	uint32_t invincibleFrame = 0;

public:
	/// @brief コンストラクタ
	BaseActObj(Camera* camera,
			   ObjModel* model,
			   const DirectX::XMFLOAT3& pos = { 0,0,0 });

	/// @brief 無敵状態の更新処理
	void invincible();

	CollisionMgr::ColliderType mycoll{}, atkcoll{};

	std::unique_ptr<ObjModel> atkModel;

#pragma region アクセッサ

	/// @return 無敵状態かどうか
	inline bool getInvincibleFrag() const { return invincibleFrag; }

	inline void setParticle(std::weak_ptr<ParticleMgr> particle) { this->particle = particle; }

	inline void setJudgeProc(const std::function<bool()>& judgeProc) { this->judge = judgeProc; }
	inline const std::function<bool()>& getJudgeProc() const { return this->judge; }

	inline void setNowBeatRaito(float num) { nowBeatRaito = num; }
	inline float getNowBeatRaito() const { return nowBeatRaito; }

	float getSpeed() { return moveSpeed; }
	void setSpeed(float speed) { moveSpeed = speed; }

	inline void setAttackFlag(bool flag) { this->attackFlag = flag; }
	inline bool getAttackFlag() const { return attackFlag; }

	inline void setDamageSe(const std::weak_ptr<SoundData>& damageSound) { damage = damageSound; }

	inline auto& getAtkObjPt() const { return atkObjPt; }

#pragma endregion アクセッサ

	//基本移動動作
	void moveProcess(const DirectX::XMFLOAT3& dir);
	void moveProcess(const DirectX::XMVECTOR& dir);

	void attackProcess();

	virtual void attack() {};
};
