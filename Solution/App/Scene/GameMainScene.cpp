#include "GameMainScene.h"
#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"
#include <CollisionMgr.h>
#include <Util/Timer.h>
#include "Input/Input.h"
// -----------------------
#include "Player/Player.h"
// -----------------------
#include <Enemy/BaseEnemy.h>
#include <3D/Obj/ObjModel.h>
#include <GameObject/GameObj.h>
#include <Camera/CameraObj.h>
#include <2D/Sprite.h>

using namespace DirectX;

GameMainScene::GameMainScene() :
	input(Input::ins()),
	spCom(std::make_unique<SpriteBase>()),
	light(std::make_unique<Light>()),
	timer(std::make_unique<Timer>()),
	bpm(120.f),
	judgeOkRange(0.25f)
{
	PostEffect::getInstance()->setAlpha(1.f);
	PostEffect::getInstance()->setMosaicNum(DirectX::XMFLOAT2(WinAPI::window_width, WinAPI::window_height));

	cameraObj = std::make_unique<CameraObj>(nullptr);
	cameraObj->easeRaito = 0.5f;

	// --------------------
	// 背景
	// --------------------
	groundModel = std::make_unique<ObjModel>("Resources/ground/", "ground");
	groundObj = std::make_unique<Object3d>(cameraObj.get(), groundModel.get());
	constexpr float groundSize = 1000.f;
	groundObj->scale = XMFLOAT3(groundSize, 1.f, groundSize);
	groundModel->setTexTilling(XMFLOAT2(groundSize, groundSize));
	groundObj->color = XMFLOAT4(1,1,1,0.5f);

	// --------------------
	// 自機
	// --------------------

	playerModel = std::make_unique<ObjModel>("Resources/cube/", "cube");
	player = std::make_unique<Player>(cameraObj.get(), playerModel.get());
	player->setHp(20u);


	cameraObj->setParentObj(player.get());

	// --------------------
	// 敵
	// --------------------
	enemyModel = std::make_unique<ObjModel>("Resources/enemy/", "enemy");
	enemy = std::make_unique<BaseEnemy>(cameraObj.get(), enemyModel.get());

	enemy->setHp(20u);
	enemy->setTargetObj(player.get());
	enemy->setPos({ 20,0,0 });
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	player->mycoll.group.emplace_front(player->createCollider());
	enemy->mycoll.group.emplace_front(enemy->createCollider());

	// タイマーの起点時間をリセット
	timer->reset();
}

void GameMainScene::update()
{
	// 拍内進行度と拍数を更新
	nowBeatRaito = Timer::calcNowBeatRaito((float)timer->getNowTime(), 120.f, nowCount);

	if (enemy->attackFlag == true)
	{
		CollisionMgr::checkHitAll(enemy->atkcoll, player->mycoll);
	}

	if (player->attackFlag == true)
	{
		player->getAtkObjPt().lock()->setCol({ 0,1,0,1 });
		CollisionMgr::checkHitAll(player->atkcoll, enemy->mycoll);
	}

	cameraObj->update();
}

void GameMainScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());

	groundObj->drawWithUpdate(light.get());

	player->drawWithUpdate(light.get());
	if (enemy->getAlive())
	{
		enemy->drawWithUpdate(light.get());
	}

	ImGui::SetNextWindowSize(ImVec2(400, 40));
	ImGui::Begin("自機",
				 nullptr,
				 DX12Base::ImGuiWinFlagsNoTitleBar
				 | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
	{
		constexpr float barWid = 400.f;
		const bool judgeRet = Timer::judge(nowBeatRaito, judgeOkRange);

		auto posLT = ImGui::GetWindowPos();
		ImVec2 posRB = posLT;
		posRB.x += barWid * nowBeatRaito;
		posRB.y += 20.f;

		ImGui::GetWindowDrawList()->AddRectFilled(posLT, posRB, ImU32(0xff2222f8));
		ImGui::Text(judgeRet ? "OK!!!" : "");
	}
	ImGui::End();
}
