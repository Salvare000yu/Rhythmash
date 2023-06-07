#pragma once
#include "BaseActObj/BaseActObj.h"

#include "EnemyBehavior.h"
#include "Collision/Collision.h"

class BaseEnemy :
	public BaseActObj
{
	friend class EnemyBehavior;

	std::unique_ptr<EnemyBehavior> enemyBehavior;

protected:
	DirectX::XMFLOAT3 dir = { 0,0,0 };

	float minX = -30.0f;
	float maxX = 30.0f;

	float minZ = -30.0f;
	float maxZ = 30.0f;

	int waitTime = 0;
	int AtkwaitTime = 0;
	int AtkTime = 0;
	bool movestop = false;

	// 攻撃対象へのポインタ
	GameObj* targetObj = nullptr;


	DirectX::XMFLOAT3 pos = { 0,0,0 };
	DirectX::XMFLOAT3 tpos = { 0,0,0 };

	std::function<void()> phase;
public:

	BaseEnemy(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });

	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	inline void setTargetObj(GameObj* obj) { targetObj = obj; }

	//ターゲットとの距離間
	float TargetFromDistance();

	inline void setPhase(const std::function<void()>& _phase) { this->phase = _phase; }

	//ターゲット方向に移動
	void MovetoTarget();
	//ランダムに移動地点を生成して移動
	void RandomMove();

	void Move()  override;
	void Attack() override;
};

