#pragma once
#include "Input/Input.h"
#include <memory>
#include "3D/Obj/ObjModel.h"
#include "GameObject/GameObj.h"
#include "Camera/CameraObj.h"
#include "3D/Light.h"

#include <DirectXMath.h>
class Player
{
	//Input* input = nullptr;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

	std::unique_ptr<Light> light;
	std::unique_ptr<CameraObj> cameraobj;
	std::unique_ptr<ObjModel> PlayerModel;
	std::unique_ptr<GameObj> PlayerObj;
private:
	bool moveZFlag = false;
	bool moveXFlag = false;
public:
	Player();
	void start();
	void update(Input* input,float val);
	void Draw();
};

