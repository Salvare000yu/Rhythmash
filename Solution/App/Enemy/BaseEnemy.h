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
	DirectX::XMFLOAT3 dir = { 0,0,0 };

	float minX = -30.0f;
	float maxX = 30.0f;

	float minZ = -30.0f;
	float maxZ = 30.0f;

	float ActTime = 0;
	int waitTime = 0;
	bool moved = false;

	float maxTargerDistance = 200.0f;
	// 攻撃対象へのポインタ
	GameObj* targetObj = nullptr;

	std::function<void()> phase;
public:

	BaseEnemy(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });

	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	inline void setTargetObj(GameObj* obj) { targetObj = obj; }

	inline float getMaxTargetDistance() const { return maxTargerDistance; }

	float TargetFromDistance();

	inline void setPhase(const std::function<void()>& _phase) { this->phase = _phase; }

	void update();
	void Move()  override;
	void Attack() override;
};

