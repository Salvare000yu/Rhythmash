#include "GamePlayScene.h"
#include <GameObject/GameObj.h>
#include <3D/Light/Light.h>
#include <3D/Obj/Object3d.h>
#include <Camera/CameraObj.h>
#include <DirectXMath.h>
#include <Input/Input.h>
#include <Collision/Collision.h>

using namespace DirectX;

GamePlayScene::GamePlayScene() :
	backState(Object3d::createGraphicsPipeline(BaseObj::BLEND_MODE::ALPHA, L"Resources/Shaders/backVS.hlsl", L"Resources/Shaders/backPS.hlsl")),
	light(std::make_unique<Light>())
{
	/*light->setPointLightActive(0, true);
	light->setPointLightColor(0, XMFLOAT3(1, 1, 1));
	light->setPointLightAtten(0, XMFLOAT3(0.3f, 0.1f, 0.1f));*/

	//light->setDirLightActive(0, true);
	//light->setDirLightColor(0, XMFLOAT3(1, 1, 1));
	//light->setDirLightDir(0, XMVectorSet(0, -1, 1, 0));	// 奥下方向

	light->setSpotLightActive(0, true);
	light->setSpotLightDir(0, XMVectorSet(0, -1, 0, 0));
	light->setSpotLightPos(0, XMFLOAT3(0, 17, 0));

	light->setCircleShadowActive(0, true);
	/*light->setCircleShadowDir(0, XMVectorSet(0, -1, 0, 0));
	light->setCircleShadowCasterPos(0, XMFLOAT3(0, 1, 0));*/
	//light->setCircleShadowCaster2LightDistance(0, 2);

	cameraObj = std::make_unique<CameraObj>(nullptr);

	groundModel = std::make_unique<ObjModel>("Resources/ground", "ground");
	groundObj = std::make_unique<Object3d>(cameraObj.get(), groundModel.get());
	groundObj->scale = XMFLOAT3(1000, 1, 1000);
	groundModel->setTexTilling(XMFLOAT2(1000, 1000));

	auto& playerModel = models.emplace("player",
				   std::make_unique<ObjModel>("Resources/player", "player")).first->second;

	auto& player =
		objs.emplace("player", std::make_unique<GameObj>((Camera*)cameraObj.get(), (ObjModel*)playerModel.get())).first->second;
	//player->setPos({ 0,0,0 });
	player->setScaleF3({ 1,1,1 });

	
	
	


	
	
	//cameraObj->setParentObj(objs.at("player").get());
	cameraObj->setParentObj(player.get());	// 上と同じ結果になる

	cameraObj->setEye2TargetLen(8.f);

	cameraObj->setEye2TargetOffset(XMFLOAT3(0, 1, 0));
}

GamePlayScene::~GamePlayScene()
{}

void GamePlayScene::update()
{
	{
		auto& player = objs.at("player");

		const bool hitRIGHT = Input::ins()->hitKey(DIK_RIGHT);
		const bool hitLEFT = Input::ins()->hitKey(DIK_LEFT);
		const bool hitUP = Input::ins()->hitKey(DIK_UP);
		const bool hitDOWN = Input::ins()->hitKey(DIK_DOWN);

		if (hitRIGHT || hitLEFT || hitUP || hitDOWN)
		{
			constexpr float speedVal = XM_2PI * 10.f;
			const float speed = speedVal / DX12Base::ins()->getFPS();

			XMFLOAT3 rotaVel{};

			if (hitRIGHT)
			{
				rotaVel.y += 1.f;
			}
			if (hitLEFT)
			{
				rotaVel.y -= 1.f;
			}
			if (hitDOWN)
			{
				rotaVel.x += 1.f;
			}
			if (hitUP)
			{
				rotaVel.x -= 1.f;
			}

			XMStoreFloat3(&rotaVel, speed * XMVector3Normalize(XMLoadFloat3(&rotaVel)));

			XMFLOAT3 rota = player->getRotation();

			rota.x += rotaVel.x;
			rota.y += rotaVel.y;
			rota.z += rotaVel.z;

			player->setRotation(rota);
		}

		const bool hitW = Input::ins()->hitKey(DIK_W);
		const bool hitA = Input::ins()->hitKey(DIK_A);
		const bool hitS = Input::ins()->hitKey(DIK_S);
		const bool hitD = Input::ins()->hitKey(DIK_D);

		if (hitW || hitA || hitS || hitD)
		{
			constexpr float speedVal = 5.f;
			const float speed = speedVal / DX12Base::ins()->getFPS();

			XMFLOAT3 vel{};

			if (hitW)
			{
				vel.z += 1.f;
			}
			if (hitS)
			{
				vel.z -= 1.f;
			}
			if (hitD)
			{
				vel.x += 1.f;
			}
			if (hitA)
			{
				vel.x -= 1.f;
			}
			XMVECTOR velVec = XMVector3Rotate(XMLoadFloat3(&vel),
							  XMQuaternionRotationRollPitchYaw(XMConvertToRadians(player->getRotation().x),
															   XMConvertToRadians(player->getRotation().y),
															   XMConvertToRadians(player->getRotation().z)));
			velVec = speed * XMVector3Normalize(XMVectorSetY(velVec, 0.f));

			XMFLOAT3 pos{};
			XMStoreFloat3(&pos, velVec + XMLoadFloat3(&player->getPos()));

			player->setPos(pos);

			light->setCircleShadowCasterPos(0, pos);
			pos.y += 16.f;
			light->setSpotLightPos(0, pos);
		}
	}

	cameraObj->update();
	for (auto& i : objs)
	{
		i.second->update();
	}
	groundObj->update();

	light->setSpotLightDir(0, XMVectorSet(0, -1, 0, 0));
	light->update();
}

void GamePlayScene::drawObj3d()
{
	for (auto& i : objs)
	{
		i.second->draw(light.get());
	}
	groundObj->draw(light.get());
}

void GamePlayScene::drawFrontSprite()
{
	ImGui::SetNextWindowSize(ImVec2(400, 260));
	ImGui::Begin("自機", nullptr, DX12Base::imGuiWinFlagsDef);
	{
		
		{
			const auto& player = objs.at("player");
			const auto& pos = player->getPos();
			ImGui::Text("自機座標：%.1f %.1f %.1f", pos.x, pos.y, pos.z);
		}
	}
	ImGui::End();
}