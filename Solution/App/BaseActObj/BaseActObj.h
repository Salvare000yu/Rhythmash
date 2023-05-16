#pragma once
#include <DirectXMath.h>
#include "GameObject/GameObj.h"
#include <CollisionMgr.h>
#include "3D/Obj/ObjModel.h"
#include "3D/ParticleMgr.h"
using namespace DirectX;

class BaseActObj
	: public GameObj
{

protected:
	//移動スピード
	float MoveSpeed = 1.5;


	//
	int WaitTime = 0;

	//float Rot = 0;
	Light* light;
	
public:
	//攻撃フラグ
	bool AttackFlag = false;
	BaseActObj(Camera* camera,
			  ObjModel* model,
			  const DirectX::XMFLOAT3& pos = { 0,0,0 });

	CollisionMgr::ColliderSet Mycoll{}, Atkcoll{};

	std::unique_ptr<ObjModel> AtkModel;
	std::unique_ptr<GameObj> AtkObj;

	std::unique_ptr<ParticleMgr> particlMgr;
	//基本移動動作
	void MoveProcess(XMFLOAT3 dir)
	{
		//前後左右のみの長さ
		float length = (dir.x * dir.x + dir.z * dir.z);
		//正規化
		float normal_x = dir.x / length;
		float normal_z = dir.z / length;

		//方向ベクトルで大まかな向いてる方向を向かせる
		{
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

		}
		//移動加算
		obj->position.x += normal_x * MoveSpeed;
		obj->position.z += normal_z * MoveSpeed;


		AtkObj->setPos({ obj->position.x ,obj->position.y,obj->position.z + 5 });

		//プレーヤーの向き回転計算
	/*	if (normal_x < 0 || normal_z < 0)
		{
			if (normal_x < 0 && normal_z < 0)
			{
				normal_x = -normal_x;
				normal_z = -normal_z;
			}

			else if (normal_x < 0)
			{
				normal_x = -normal_x;
			} else if (normal_z < 0)
			{
				normal_z = -normal_z;
			}
		}
		Rot = normal_x / normal_z;
		Rot = atanf(Rot);
		Rot = Rot * (180 / XM_PI);
		this->setRotation({ 0,Rot,0 });*/
	}


	void AttackProcess()
	{
		
		Mycoll.hitProc = [&](GameObj* obj)
		{
			this->setCol({ 1,0,0,1 });
			particlMgr->createParticle(particlMgr.get(), this->getPos(),50);
			if (obj->damage(1u, false))
			{
				
				obj->kill();
				return;
			}
		};
		this->setCol({ 1,1,1,1 });

		if (AttackFlag == true)
		{
			WaitTime++;
			if (WaitTime >= 30)
			{
				AttackFlag = false;
				AtkObj->setCol({ 1,1,1,1 });
				WaitTime = 0;
			}
		}
	}

	virtual ~BaseActObj() {}
	virtual void Move() = 0;
	virtual void Attack() = 0;
};

