#include "GamePlayScene.h"
#include <GameObject/GameObj.h>
#include <3D/Light.h>
#include <3D/Obj/Object3d.h>
#include <Camera/CameraObj.h>
#include <DirectXMath.h>
#include <Input/Input.h>

using namespace DirectX;

GamePlayScene::GamePlayScene() :
	backState(Object3d::createGraphicsPipeline(BaseObj::BLEND_MODE::ALPHA, L"Resources/Shaders/backVS.hlsl", L"Resources/Shaders/backPS.hlsl")),
	light(std::make_unique<Light>())
{
	light->setLightPos({ 0, 0.5f, 0 });
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
		}
	}

	cameraObj->update();
	light->setLightPos(cameraObj->getEye());
	for (auto& i : objs)
	{
		i.second->update();
	}
	groundObj->update();
}

void GamePlayScene::drawObj3d()
{
	for (auto& i : objs)
	{
		i.second->draw(light.get());
	}
	groundObj->draw(light.get(), backState);
}

void GamePlayScene::drawFrontSprite()
{
	ImGui::SetNextWindowSize(ImVec2(300, 200));
	ImGui::Begin("自機", nullptr, DX12Base::imGuiWinFlagsDef);
	{
		const auto& pWPos = objs.at("player")->calcWorldPos();
		ImGui::Text("ワールド座標：%.1f %.1f %.1f", pWPos.x, pWPos.y, pWPos.z);
	}
	ImGui::End();
}
