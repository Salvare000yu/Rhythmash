#pragma once
#include <DirectXMath.h>
#include "GameObject/GameObj.h"
#include <CollisionMgr.h>
#include "3D/Obj/ObjModel.h"
#include <Sound/Sound.h>

class Light;
class Camera;
class ParticleMgr;

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
public:
	//攻撃フラグ
	BaseActObj(Camera* camera,
			   ObjModel* model,
			   const DirectX::XMFLOAT3& pos = { 0,0,0 });

	CollisionMgr::ColliderType mycoll{}, atkcoll{};

	std::unique_ptr<ObjModel> atkModel;

#pragma region アクセッサ

	inline void setParticle(std::weak_ptr<ParticleMgr> particle) { this->particle = particle; }

	inline void setJudgeProc(const std::function<bool()>& judgeProc) { this->judge = judgeProc; }
	inline const std::function<bool()>& getJudgeProc() const { return this->judge; }

	inline void setNowBeatRaito(float num) { nowBeatRaito = num; }
	inline float getNowBeatRaito() const { return nowBeatRaito; }

	float getSpeed() { return moveSpeed; }
	void setSpeed(float speed) { moveSpeed = speed; }

	inline bool getAttackFlag() const { return attackFlag; }

	inline void setDamageSe(const std::weak_ptr<SoundData>& damageSound) { damage = damageSound; }

	inline auto& getAtkObjPt() const { return atkObjPt; }

#pragma endregion アクセッサ

	//基本移動動作
	void MoveProcess(const DirectX::XMFLOAT3& dir);
	void MoveProcess(const DirectX::XMVECTOR& dir);

	void AttackProcess();

	virtual void Move() {};
	virtual void Attack() {};
};
