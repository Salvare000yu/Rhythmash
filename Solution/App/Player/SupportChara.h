#pragma once
#include "BaseActObj/BaseActObj.h"

class SupportChara
	: public BaseActObj
{
private:
	GameObj* player = nullptr;

	DirectX::XMFLOAT3 dir = { 0,0,0 };
	DirectX::XMFLOAT3 pos = {};
	bool Rev = false;
	int RevCount = 0;
public:
	SupportChara(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });
	void Move()  override;
	void SetDir(DirectX::XMFLOAT3 Dir) { dir = Dir; }

	void Step();
};

