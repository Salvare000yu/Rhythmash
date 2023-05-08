#pragma once
#include <DirectXMath.h>
#include "GameObject/GameObj.h"
#include <CollisionMgr.h>

using namespace DirectX;

class BaseActObj
	: public GameObj
{

protected:
	//移動スピード
	float MoveSpeed = 1.5;
	//攻撃フラグ
	bool AttackFlag = false;

	//無敵時間
	int WaitTime = 0;
public:

	BaseActObj(Camera* camera,
			  ObjModel* model,
			  const DirectX::XMFLOAT3& pos = { 0,0,0 });

	CollisionMgr::ColliderSet mycoll{};

	//基本移動動作
	void MoveProcess(XMFLOAT3 dir)
	{
		//前後左右のみの長さ
		float length = (dir.x * dir.x + dir.z * dir.z);
		//正規化
		float normal_x = dir.x / length;
		float normal_z = dir.z / length;

		obj->position.x += normal_x * MoveSpeed;
		obj->position.z += normal_z * MoveSpeed;
	}

	void AttackProcess()
	{
		mycoll.hitProc = [&](GameObj* obj)
		{
			if (AttackFlag == false)
			{

				this->setCol({ 1,0,0,1 });
				if (obj->damage(1u, false))
				{
					obj->kill();
					return;
				}

			}
		};
		this->setCol({ 1,1,1,1 });

	}
	void OnCollision()
	{

	}
	virtual ~BaseActObj() {}
	virtual void Move() = 0;
	virtual void Attack() = 0;
};

