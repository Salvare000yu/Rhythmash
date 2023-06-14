#pragma once
#include "BaseActObj/BaseActObj.h"
#include <DirectXMath.h>
#include <functional>
#include <Sound/Sound.h>
#include <memory>

class InputMgr;
class CameraObj;

class Player
	: public BaseActObj
{
	InputMgr* inputMgr = nullptr;
	CameraObj* cameraObj = nullptr;
	std::unique_ptr<Sound> se1;

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
	inline auto createCollider() { return CollisionMgr::ColliderType{.obj = this, .colliderR = this->getScaleF3().z }; }

	void Move()  override;
	void Attack() override;
	void Step();
	void ViewShift();
	void setCameraObj(CameraObj* cameraObj) { this->cameraObj = cameraObj; }
};

