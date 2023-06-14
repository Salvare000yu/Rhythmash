#pragma once
#include <DirectXMath.h>
#include "GameObject/GameObj.h"
#include <CollisionMgr.h>
#include "3D/Obj/ObjModel.h"
#include <3D/ParticleMgr.h>
#include <Sound/Sound.h>
class Light;
class Camera;

class BaseActObj
	: public GameObj
{
public:
	static constexpr float moveSpeedDef = 15.f;

private:
	std::unique_ptr<ParticleMgr> particleMgr;
	std::weak_ptr<Sound> damage;

	float nowBeatRaito = 0.f;

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

	CollisionMgr::ColliderSet mycoll{}, atkcoll{};

	std::unique_ptr<ObjModel> atkModel;

	inline void setJudgeProc(const std::function<bool()>& judgeProc) { this->judge = judgeProc; }
	inline const std::function<bool()>& getJudgeProc() const { return this->judge; }

	inline void setNowBeatRaito(float num) { nowBeatRaito = num; }
	inline float getNowBeatRaito() const { return nowBeatRaito; }

	inline bool getAttackFlag() const { return attackFlag; }

	inline void setDamageSe(const std::weak_ptr<Sound>& damageSound) { damage = damageSound; }

	inline auto& getAtkObjPt() const { return atkObjPt; }

	//基本移動動作
	void MoveProcess(const DirectX::XMFLOAT3& dir);
	void MoveProcess(const DirectX::XMVECTOR& dir);

	void AttackProcess();

	virtual void Move() {};
	virtual void Attack() {};

	float GetSpeed() { return moveSpeed; }
	void SetSpeed(float speed) { moveSpeed = speed; }
};

