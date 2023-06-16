#pragma once
#include "BaseActObj/BaseActObj.h"
#include <DirectXMath.h>
#include <functional>
#include <Sound/Sound.h>

class Input;

class Player
	: public BaseActObj
{
	Input* input = nullptr;
	std::weak_ptr<SoundData> se1;

	std::function<void()> update_proc;

	DirectX::XMFLOAT3 dir = { 0,0,0 };

	int AttackFrame = 0;

	float normalSpeed = moveSpeedDef;
	float dashSpeed = moveSpeedDef;
	float dashSpeedAttenuation = 1.f;

private:
	bool loadYamlFile();

public:
	Player(Camera* camera,
		   ObjModel* model,
		   const DirectX::XMFLOAT3& pos = { 0,0,0 });

	void Move()  override;
	void Attack() override;
	void Step();
};
