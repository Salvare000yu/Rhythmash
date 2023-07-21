#pragma once
#include "BaseActObj/BaseActObj.h"

class SupportChara
	: public BaseActObj
{
private:
	GameObj* player = nullptr;
	std::unique_ptr<ParticleMgr> particleMgr;
	DirectX::XMFLOAT3 dir = { 0,0,0 };
	DirectX::XMFLOAT3 pos = {};
	bool Rev = false;
	int RevCount = 0;

	int preBeatCount = 0;

	int nowBeatCount = 0;
	int MoveCount = 0;
	int MaxCount = 3;
public:
	SupportChara(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });
	void Move()  override;
	void SetDir(DirectX::XMFLOAT3 Dir) { dir = Dir; }

	inline void setPreBeatCount(int count) { preBeatCount = count; };

	void Step();
};

