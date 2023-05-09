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

	GameObj* Atkobj;
	//基本移動動作
	void MoveProcess(XMFLOAT3 dir)
	{
		//前後左右のみの長さ
		float length = (dir.x * dir.x + dir.z * dir.z);
		//正規化
		float normal_x = dir.x / length;
		float normal_z = dir.z / length;

		if (normal_x > 0 || normal_z > 0)
		{
			//WDキー同時　右上
			if (normal_x > 0 && normal_z > 0)
			{
				this->setRotation({ 0,35,0 });
			}
			//Dキー　右
			else if (normal_x > 0)
			{
				this->setRotation({ 0,90,0 });
			}
			//Wキー　前
			else if (normal_z > 0)
			{
				this->setRotation({ 0,0,0 });
			}
		}

		else if (normal_x < 0 || normal_z < 0)
		{
			//ASキー同時　左下
			if (normal_x < 0 && normal_z < 0)
			{
				this->setRotation({ 0,-145,0 });
			}
			//Aキー　左
			else if (normal_x < 0)
			{
				this->setRotation({ 0,-90,0 });
			}
			//Sキー　下
			else if (normal_z < 0)
			{
				this->setRotation({ 0,-180,0 });
			}
		}
		//AWキー同時　左上
		if (normal_x < 0 && normal_z > 0)
		{
			this->setRotation({ 0,-35,0 });
		}
		//SDキー同時 右下
		else if (normal_x > 0 && normal_z < 0)
		{
			this->setRotation({ 0,145,0 });
		}
		obj->position.x += normal_x * MoveSpeed;
		obj->position.z += normal_z * MoveSpeed;
	}

	void AttackProcess()
	{
		/*CollisionMgr::ColliderSet AtkColl{};
		AtkColl.group.emplace_front();*/
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

	virtual ~BaseActObj() {}
	virtual void Move() = 0;
	virtual void Attack() = 0;
};

