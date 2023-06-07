#include "GameMainScene.h"
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
#include "Util/Util.h"
#include"Sound/Sound.h"

#include <Scene/GameOverScene.h>
#include <Scene/GameClearScene.h>
using namespace DirectX;

GameMainScene::GameMainScene() :
	input(Input::ins()),
	light(std::make_unique<Light>()),
	timer(std::make_unique<Timer>()),
	bpm(100.f),
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
	enemy = std::make_unique<BaseEnemy>(cameraObj.get(), enemyModel.get());

	enemy->setHp(20u);
	enemy->setTargetObj(player.get());
	enemy->setPos({ 20,0,0 });

	light.reset(new Light());

	stageModel = std::make_unique<ObjModel>("Resources/ring/", "ring");
	stageObj = std::make_unique<GameObj>(cameraObj.get(), stageModel.get());

	stageObj->setScale(10);
	stageObj->setRotation({ 0,90,0 });
	stageObj->setPos({ 0,0,0 });
	//csvの読み込み
	const std::vector<std::string> fileNames = { "Resources/Csv/enemy.csv","Resources/Csv/player.csv" };
	Util::CSVType csvData = Util::loadCsv(fileNames, true, ',', "//");

	struct CavDataFormat
	{
		std::string type = "";
		uint16_t hp = 0ui16;
		uint16_t attack = 0ui16;
		std::forward_list<XMFLOAT3> pos{};
	};
	std::forward_list<CavDataFormat> loadedCsvData;
	CavDataFormat* currentData = nullptr;

	for (const auto& i : csvData)
	{
		if (i[0] == "type")
		{
			loadedCsvData.emplace_front(CavDataFormat{ .type = i[1] });
			currentData = &loadedCsvData.front();
		} else if (currentData)
		{
			if (i[0] == "position")
			{
				currentData->pos.emplace_front(XMFLOAT3(std::stof(i[1]),
														std::stof(i[2]),
														std::stof(i[3])));
			} else if (i[0] == "hp")
			{
				currentData->hp = static_cast<uint16_t>(std::stoul(i[1]));
			}
		}
	}

	for (const auto& i : loadedCsvData)
	{
		if (i.type == "player")
		{
			player->setAttack(i.attack);
			player->setHp(i.hp);
			player->setPos(i.pos.front());
		} else if (i.type == "enemy")
		{
			for (const auto& e : i.pos)
			{
				// todo 敵の複数化に対応

				enemy->setAttack(i.attack);
				enemy->setHp(i.hp);
				enemy->setPos(e);
			}
		}
	}

	sound = std::make_unique<Sound>("Resources/SE/practiseBGM.wav");
}
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

	Sound::SoundPlayWave(sound.get(), XAUDIO2_LOOP_INFINITE);

	// タイマーの起点時間をリセット
	timer->reset();
}

void GameMainScene::update()
{
	// 拍内進行度と拍数を更新
	nowBeatRaito = Timer::calcNowBeatRaito((float)timer->getNowTime(), bpm, nowCount);
	for (auto& i : enemy)
	{
		if (i->attackFlag)
		{
			CollisionMgr::checkHitAll(i->atkcoll, player->mycoll);
		}

		if (player->attackFlag)
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

	stageObj->drawWithUpdate(light.get());
	player->drawWithUpdate(light.get());
	for (auto& i : enemy)
	{
		if (!i->getAlive()) { continue; }
		i->drawWithUpdate(light.get());
	}
}

void GameMainScene::drawFrontSprite()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400));
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

	ImGui::Text("[WASD]: 移動");
	ImGui::Text("移動 + リズムよく[C]: ダッシュ");
	ImGui::Text("リズムよく[スペース]: 前方に攻撃");

	ImGui::End();
}

void GameMainScene::addEnemy(const DirectX::XMFLOAT3& pos)
{
	auto& i = enemy.emplace_front(std::make_shared<BaseEnemy>(cameraObj.get(), enemyModel.get()));

	i->setHp(3u);
	i->setTargetObj(player.get());
	i->setPos(pos);
}

