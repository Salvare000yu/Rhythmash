#pragma once
#include "BaseActObj/BaseActObj.h"
#include <memory>
#include <DirectXMath.h>

class Sequencer;

class BaseEnemy :
	public BaseActObj
{
protected:
	DirectX::XMFLOAT3 dir = { 0,0,0 };

	float minX = -30.0f;
	float maxX = 30.0f;

	float minZ = -30.0f;
	float maxZ = 30.0f;

	uint16_t actFrame = 0;
	uint16_t waitFrame = 0;
	bool moved = false;

	std::unique_ptr<Sequencer> attackProc;

public:
	BaseEnemy(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });

	inline auto createCollider() { return CollisionMgr::ColliderType{ .obj = this, .colliderR = this->getScaleF3().z }; }

	void additionalUpdate() override;
	void Move()  override;
	void Attack() override;
};

