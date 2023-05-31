#pragma once
#include "BaseActObj/BaseActObj.h"
#include <memory>
#include "EnemyBehavior.h"
#include "Collision/Collision.h"
class BaseEnemy :
	public BaseActObj
{
	friend class EnemyBehavior;

	std::unique_ptr<EnemyBehavior> enemyBehavior;

protected:
	XMFLOAT3 dir = { 0,0,0 };

	float minX = -30.0f;
	float maxX = 30.0f;

	float minZ = -30.0f;
	float maxZ = 30.0f;

	int AtkWaitTime = 0;
	int AttackTime = 0;

	int MoveWaitTime = 0;
	bool movestop = false;

	// 攻撃対象へのポインタ
	GameObj* targetObj = nullptr;


	XMFLOAT3 pos = { 0,0,0 };
	XMFLOAT3 tpos = { 0,0,0 };

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

	void update();
	//ターゲット方向に移動
	void MovetoTarget();
	//ランダムに移動地点を生成して移動
	void RandomMove();

	void Move()  override;
	void Attack() override;
};

