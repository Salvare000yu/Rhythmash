#pragma once
#include <DirectXMath.h>
#include "GameObject/GameObj.h"
class BaseActObj
	: public GameObj
{
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

protected:
	//移動スピード
	float MoveSpeed = 1.5;
public:

	BaseActObj(Camera* camera,
			  ObjModel* model,
			  const DirectX::XMFLOAT3& pos = { 0,0,0 });
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

	}
	virtual ~BaseActObj() {}
	virtual void Move() = 0;
	virtual void Attack() = 0;
};

