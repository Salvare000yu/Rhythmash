#pragma once
#include <DirectXMath.h>
#include "GameObject/GameObj.h"
#include <CollisionMgr.h>
#include "3D/Obj/ObjModel.h"
#include <3D/ParticleMgr.h>

class Light;
class Camera;

class BaseActObj
	: public GameObj
{
public:
	static constexpr float moveSpeedDef = 15.f;

private:
	std::unique_ptr<ParticleMgr> particleMgr;

protected:
	//移動スピード
	float moveSpeed = moveSpeedDef;

	int waitFrame = 0;

	//float Rot = 0;
	Light* light;

	std::weak_ptr<GameObj> atkObjPt;
public:
	//攻撃フラグ
	bool attackFlag = false;
	BaseActObj(Camera* camera,
			  ObjModel* model,
			  const DirectX::XMFLOAT3& pos = { 0,0,0 });

	CollisionMgr::ColliderSet mycoll{}, atkcoll{};

	std::unique_ptr<ObjModel> atkModel;

	inline auto& getAtkObjPt() const { return atkObjPt; }

	//基本移動動作
	void MoveProcess(const DirectX::XMFLOAT3& dir);
	void MoveProcess(const DirectX::XMVECTOR& dir);

	void AttackProcess();

	virtual ~BaseActObj() {}
	virtual void Move() = 0;
	virtual void Attack() = 0;

	float GetSpeed() { return moveSpeed; }
	void SetSpeed(float speed) { moveSpeed = speed; }
};

