#include "GameMainScene.h"
#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"
#include <CollisionMgr.h>
#include <Util/Timer.h>
#include "Input/Input.h"

#include "Player/Player.h"
#include "Player/SupportChara.h"
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
	// 音
	// --------------------
	sound = std::make_unique<Sound>("Resources/SE/practiseBGM.wav");
	damageSe = std::make_shared<Sound>("Resources/SE/damage.wav");

	// --------------------
	// 自機
	// --------------------

	playerModel = std::make_unique<ObjModel>("Resources/cube/", "cube");
	player = std::make_unique<Player>(cameraObj.get(), playerModel.get());
	player->setDamageSe(damageSe);
	player->setJudgeProc([&] { return Timer::judge(player->getNowBeatRaito(), judgeOkRange); });

	cameraObj->setParentObj(player.get());

	SpcharaModel= std::make_unique<ObjModel>("Resources/box/", "box");
	Spchara = std::make_unique<SupportChara>(cameraObj.get(), SpcharaModel.get());
	Spchara->setJudgeProc([&] { return Timer::judge(player->getNowBeatRaito(), judgeOkRange); });
	Spchara->setPos({2,0,-3});
	Spchara->setParent(player.get()->getObj());
	// --------------------
	// 敵
	// --------------------
	enemyModel = std::make_unique<ObjModel>("Resources/enemy/", "enemy");

	light.reset(new Light());

	stageModel = std::make_unique<ObjModel>("Resources/ring/", "ring");
	stageObj = std::make_unique<GameObj>(cameraObj.get(), stageModel.get());

	stageObj->setScale(10);
	stageObj->setRotation({ 0,90,0 });
	stageObj->setPos({ 0,0,0 });
	//csvの読み込み
	const std::vector<std::string> fileNames = { "Resources/DataFile/enemy.csv" };
	Util::CSVType csvData = Util::loadCsvs(fileNames, true, ',', "//");

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
		if (i.type == "enemy")
		{
			for (const auto& e : i.pos)
			{
				auto tmp = addEnemy(e).lock();
				tmp->setAttack(i.attack);
				tmp->setHp(i.hp);
			}
		}
	}

	for (auto& i : enemy)
	{
		i->setJudgeProc([&] { return Timer::judge(i->getNowBeatRaito(), judgeOkRange); });
	}
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	player->mycoll.group.emplace_front(player->createCollider());
	for (auto& i : enemy)
	{
		i->mycoll.group.emplace_front(CollisionMgr::ColliderType::create(i.get()));
	}

	Sound::SoundPlayWave(sound.get(), XAUDIO2_LOOP_INFINITE);

	// タイマーの起点時間をリセット
	timer->reset();

	
}

void GameMainScene::update()
{
	Spchara->Move();
	// 拍内進行度と拍数を更新
	nowBeatRaito = Timer::calcNowBeatRaito((float)timer->getNowTime(), bpm, nowCount);
	for (auto& i : enemy)
	{
		if (i->getAttackFlag())
		{
			CollisionMgr::checkHitAll(i->atkcoll, player->mycoll);
		}

		if (player->getAttackFlag())
		{
			CollisionMgr::checkHitAll(player->atkcoll, i->mycoll);
		}
	}
	enemy.remove_if([](const std::shared_ptr<BaseEnemy>& e) { return !e->getAlive(); });

	{
		const float raitoColor = std::lerp(0.25f, 1.f, 1.f - nowBeatRaito);
		player->setCol(XMFLOAT4(raitoColor, raitoColor, raitoColor, 1.f));

		player->setNowBeatRaito(nowBeatRaito);
		for (auto& i : enemy)
		{
			i->setNowBeatRaito(nowBeatRaito);
			i->setNowBeatCount((uint32_t)nowCount);
		}
	}

	//シーン移行
	if (!player->getAlive())
	{
		SceneManager::getInstange()->changeScene<GameOverScene>();
	} else if (enemy.empty())
	{
		SceneManager::getInstange()->changeScene<GameClearScene>();
	}

	cameraObj->update();
}

void GameMainScene::drawObj3d()
{
	groundObj->drawWithUpdate(light.get());

	stageObj->drawWithUpdate(light.get());
	player->drawWithUpdate(light.get());
	Spchara->drawWithUpdate(light.get());
	for (auto& i : enemy)
	{
		if (!i->getAlive()) { continue; }
		i->drawWithUpdate(light.get());
	}
}

void GameMainScene::drawFrontSprite()
{
	constexpr float barWid = 300.f;
	ImGui::SetNextWindowSize(ImVec2(barWid, 150));
	ImGui::Begin("自機",
				 nullptr,
				 DX12Base::ImGuiWinFlagsNoTitleBar
				 | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);

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

	ImGui::SetNextWindowSize(ImVec2(100, 100));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	enemy.front()->drawIdmGui();
}

std::weak_ptr<BaseEnemy> GameMainScene::addEnemy(const DirectX::XMFLOAT3& pos)
{
	auto& i = enemy.emplace_front(std::make_shared<BaseEnemy>(cameraObj.get(), enemyModel.get()));

	i->setHp(3u);
	i->setTargetObj(player.get());
	i->setPos(pos);
	i->setDamageSe(damageSe);

	return i;
}

