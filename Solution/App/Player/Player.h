#pragma once
#include "BaseActObj/BaseActObj.h"
#include "Input/Input.h"
#include <memory>
#include "3D/Obj/ObjModel.h"
#include "GameObject/GameObj.h"
#include "Camera/CameraObj.h"
#include "3D/Light.h"
#include <DirectXMath.h>
class Player
	: public BaseActObj
{

	std::unique_ptr<Light> light;
	std::unique_ptr<CameraObj> cameraobj;
	std::unique_ptr<ObjModel> PlayerModel;
	std::unique_ptr<GameObj> PlayerObj;

	Input* input = nullptr;

	DirectX::XMFLOAT3 dir = { 0,0,0 };

	bool isWeekAttack;
	bool isHardAttack;
	bool isJump;
	bool isStep;

public:
	Player(Camera* camera,
				ObjModel* model,
				const DirectX::XMFLOAT3& pos = { 0,0,0 });
	void start();
	void update();
	void Draw();

	void Move()  override;
	void Attack() override;
};

