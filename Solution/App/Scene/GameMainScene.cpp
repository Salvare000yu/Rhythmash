﻿#include "GameMainScene.h"
#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"
#include <CollisionMgr.h>
#include <Util/Timer.h>
#include "Input/Input.h"

#include "Player/Player.h"

#include <Enemy/BaseEnemy.h>
#include <3D/Obj/ObjModel.h>
#include <GameObject/GameObj.h>
#include <Camera/CameraObj.h>
#include <Scene/GameOverScene.h>
#include <Scene/GameClearScene.h>
using namespace DirectX;

GameMainScene::GameMainScene() :
	input(Input::ins()),
	light(std::make_unique<Light>()),
	timer(std::make_unique<Timer>()),
	bpm(120.f),
	judgeOkRange(0.25f)
{
	PostEffect::getInstance()->setAlpha(1.f);
	PostEffect::getInstance()->setMosaicNum(DirectX::XMFLOAT2(WinAPI::window_width, WinAPI::window_height));

	cameraObj = std::make_unique<CameraObj>(nullptr);
	cameraObj->easeRaito = 0.5f;
	cameraObj->useParentRotaFlag = false;

	// --------------------
	// 背景
	// --------------------
	groundModel = std::make_unique<ObjModel>("Resources/ground/", "ground");
	groundObj = std::make_unique<Object3d>(cameraObj.get(), groundModel.get());
	constexpr float groundSize = 1000.f;
	groundObj->scale = XMFLOAT3(groundSize, 1.f, groundSize);
	groundModel->setTexTilling(XMFLOAT2(groundSize, groundSize));
	groundObj->color = XMFLOAT4(1, 1, 1, 0.5f);

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
	for (int i = 1; i < 4; i++)
	{
		XMFLOAT3 pos = { 15 * (float)i,0,20 * (float)i };
		addEnemy(pos);
	}
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	player->mycoll.group.emplace_front(player->createCollider());
	for (auto& i : enemy)
	{
		i->mycoll.group.emplace_front(i->createCollider());
	}


	// タイマーの起点時間をリセット
	timer->reset();
}

void GameMainScene::update()
{
	// 拍内進行度と拍数を更新
	nowBeatRaito = Timer::calcNowBeatRaito((float)timer->getNowTime(), bpm, nowCount);
	for (auto& i : enemy)
	{
		if (i->attackFlag == true)
		{
			CollisionMgr::checkHitAll(i->atkcoll, player->mycoll);
		}

		if (player->attackFlag == true)
		{
			CollisionMgr::checkHitAll(player->atkcoll, i->mycoll);
		}
	}

	{
		const float raitoColor = std::lerp(0.25f, 1.f, 1.f - nowBeatRaito);
		player->setCol(XMFLOAT4(raitoColor, raitoColor, raitoColor, 1.f));

		const bool judgeRet = Timer::judge(nowBeatRaito, judgeOkRange);
		player->setJudge(judgeRet);
	}

	cameraObj->update();
	//シーン移行
	if (!player->getAlive())
	{
		SceneManager::getInstange()->changeScene<GameOverScene>();
	}

	for (const auto& i : enemy)
	{
		NonEnemy = !i->getAlive();
		if (!NonEnemy) { break; }
		if (NonEnemy)
		{
			SceneManager::getInstange()->changeScene<GameClearScene>();
		}
	}
}

void GameMainScene::drawObj3d()
{
	groundObj->drawWithUpdate(light.get());

	player->drawWithUpdate(light.get());
	for (auto& i : enemy)
	{
		if (!i->getAlive()) { continue; }
		i->drawWithUpdate(light.get());
	}
}

void GameMainScene::drawFrontSprite()
{
	ImGui::SetNextWindowSize(ImVec2(400, 40));
	ImGui::Begin("自機",
				 nullptr,
				 DX12Base::ImGuiWinFlagsNoTitleBar
				 | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);

	constexpr float barWid = 400.f;
	const bool judgeRet = Timer::judge(nowBeatRaito, judgeOkRange);

	auto posLT = ImGui::GetWindowPos();
	ImVec2 posRB = posLT;
	posRB.x += barWid * nowBeatRaito;
	posRB.y += 20.f;

	ImGui::GetWindowDrawList()->AddRectFilled(posLT, posRB, ImU32(0xff2222f8));
	ImGui::Text(judgeRet ? "OK!!!" : "");

	ImGui::End();
}

void GameMainScene::addEnemy(const DirectX::XMFLOAT3& pos)
{
	auto& i = enemy.emplace_front(std::make_shared<BaseEnemy>(cameraObj.get(), enemyModel.get()));

	i->setHp(3u);
	i->setTargetObj(player.get());
	i->setPos(pos);
}

