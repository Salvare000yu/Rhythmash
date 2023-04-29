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

	light->setDirLightActive(0, true);
	light->setDirLightColor(0, XMFLOAT3(1, 1, 1));
	light->setDirLightDir(0, XMVectorSet(0, -1, 0, 0));

	/*light->setSpotLightActive(0, true);
	light->setSpotLightDir(0, XMVectorSet(0, -1, 0, 0));
	light->setSpotLightPos(0, XMFLOAT3(0, 2, 0));*/

	light->setCircleShadowActive(0, true);
	light->setCircleShadowDir(0, XMVectorSet(0, -1, 0, 0));
	light->setCircleShadowCasterPos(0, XMFLOAT3(0, 1, 0));
	light->setCircleShadowCaster2LightDistance(0, 2);

	cameraObj = std::make_unique<CameraObj>(nullptr);

	groundModel = std::make_unique<ObjModel>("Resources/ground", "ground");
	groundObj = std::make_unique<Object3d>(cameraObj.get(), groundModel.get());
	groundObj->scale = XMFLOAT3(1000, 1, 1000);
	groundModel->setTexTilling(XMFLOAT2(1000, 1000));

	auto& playerModel = models.emplace("player",
				   std::make_unique<ObjModel>("Resources/player", "player")).first->second;

	auto& player =
		objs.emplace("player", std::make_unique<GameObj>((Camera*)cameraObj.get(), (ObjModel*)playerModel.get())).first->second;
	player->setPos({ 0,1,0 });
	player->setScaleF3({ 1,1,1 });

	//cameraObj->setParentObj(objs.at("player").get());
	cameraObj->setParentObj(player.get());	// 上と同じ結果になる

	cameraObj->setEye2TargetLen(10.f);

	cameraObj->setEye2TargetOffset(XMFLOAT3(0, 0, 0));
}

GamePlayScene::~GamePlayScene()
{}

void GamePlayScene::update()
{
	{
		const bool hitW = Input::ins()->hitKey(DIK_W);
		const bool hitA = Input::ins()->hitKey(DIK_A);
		const bool hitS = Input::ins()->hitKey(DIK_S);
		const bool hitD = Input::ins()->hitKey(DIK_D);

		if (hitW || hitA || hitS || hitD)
		{
			auto& player = objs.at("player");

			XMFLOAT3 pos = player->getPos();

			constexpr float speedVal = 5.f;
			const float speed = speedVal / DX12Base::ins()->getFPS();
			if (hitW)
			{
				pos.z += speed;
			}
			if (hitS)
			{
				pos.z -= speed;
			}
			if (hitD)
			{
				pos.x += speed;
			}
			if (hitA)
			{
				pos.x -= speed;
			}

			player->setPos(pos);
			light->setCircleShadowCasterPos(0, pos);
		}
	}

	cameraObj->update();
	for (auto& i : objs)
	{
		i.second->update();
	}
	groundObj->update();

	{
		XMFLOAT3 pos = objs.at("player")->calcWorldPos();
		pos.y += 3.f;
		light->setSpotLightPos(0, pos);
		light->setPointLightPos(0, pos);
	}
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
	ImGui::SetNextWindowSize(ImVec2(400, 200));
	ImGui::Begin("自機", nullptr, DX12Base::imGuiWinFlagsDef);
	{
		{
			const auto& pWPos = objs.at("player")->calcWorldPos();
			ImGui::Text("自機ワールド座標：%.1f %.1f %.1f", pWPos.x, pWPos.y, pWPos.z);
		}
		{
			const auto& camPos = cameraObj->getEye();
			ImGui::Text("カメラワールド座標：%.1f %.1f %.1f", camPos.x, camPos.y, camPos.z);
		}
		{
			XMFLOAT3 dir{};
			XMStoreFloat3(&dir, light->getCircleShadowDir(0));
			float num[3]{ dir.x,dir.y,dir.z };
			ImGui::SliderFloat3("丸影向", num, -1.f, 1.f);
			if (num[0] != dir.x ||
				num[1] != dir.y ||
				num[2] != dir.z)
			{
				light->setCircleShadowDir(0, XMLoadFloat3(&XMFLOAT3(num)));
			}
		}
		{
			const auto& att = light->getCircleShadowAtten(0);
			float num[3]{ att.x,att.y,att.z };
			ImGui::SliderFloat3("丸影減衰", num, 0.f, 1.f);
			if (num[0] != att.x ||
				num[1] != att.y ||
				num[2] != att.z)
			{
				light->setCircleShadowAtten(0, XMFLOAT3(num));
			}
		}
	}
	ImGui::End();
}
