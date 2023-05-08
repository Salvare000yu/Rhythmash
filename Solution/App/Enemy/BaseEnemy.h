#pragma once
#include "BaseActObj/BaseActObj.h"
#include <memory>
#include <DirectXMath.h>
class BaseEnemy :
	public BaseActObj
{
protected:
	DirectX::XMFLOAT3 dir = { 0,0,0 };

	float minX = -30.0f;
	float maxX = 30.0f;

	float minZ = -30.0f;
	float maxZ = 30.0f;

	float ActTime = 0;
	int waitTime = 0;
	bool moved = false;
public:
	BaseEnemy(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });

	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	void update();
	void Move()  override;
	void Attack() override;
};

