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

	
	
	/*hpBar = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/hp.png"),
									 spCom.get(),
							 XMFLOAT2(0.f, 0.f));
	hpBar->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));*/

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


	StageModel = std::make_unique<ObjModel>("Resources/ring/", "ring");
	StageObj = std::make_unique<GameObj>(cameraObj.get(), StageModel.get());

	StageObj->setScale({ 10 });
	StageObj->setRotation({ 0,90,0 });
	StageObj->setPos({ 0,0,0 });
	//csvの読み込み
	const std::vector<std::string> fileNames = { "Resources/Csv/enemy.csv","Resources/Csv/player.csv" };
	Util::CSVType csvData = Util::loadCsv(fileNames, true, ',', "//");
	
	XMFLOAT3 csvpos{};
	std::vector<XMFLOAT3> enemypos;
	std::vector<XMFLOAT3> enemypos2;
	std::vector<XMFLOAT3> enemypos3;
	XMFLOAT3 playerpos;
	std::string currentType;
	float hp = 0.0f;
	float attack = 0.0f;


	for (size_t i = 0; i < csvData.size(); i++)
	{
		
		if (csvData[i][0] == "type")
		{
			currentType = csvData[i][1];
			continue;

		} else if (csvData[i][0] == "position")
		{
			csvpos.x = std::stof(csvData[i][1]);
			csvpos.y = std::stof(csvData[i][2]);
			csvpos.z = std::stof(csvData[i][3]);
		} else if (csvData[i][0] == "hp")
		{
			hp = std::stof(csvData[i][1]);
			continue;
		}else if (csvData[i][0] == "attack")
		{
			attack = std::stof(csvData[i][1]);
			continue;
		}

		if (currentType == "enemy")
		{
			enemypos.push_back(csvpos);
			if (hp > 0) { enemy->setHp(static_cast<uint16_t>(hp)); }
			if (attack > 0) { enemy->setAttack(static_cast<uint16_t>(attack)); }
			hp = 0;
			attack = 0;
			
		}/* else if (currentType == "enemy2")
		{
			enemypos2.push_back(csvpos);
			if (hp > 0) { enemy2->setHp(static_cast<uint16_t>(hp)); }
			if (attack > 0) { enemy2->setAttack(static_cast<uint16_t>(attack)); }
			hp = 0;
			attack = 0;
		}*//* else if (currentType == "enemy3")
		{
			enemypos3.push_back(csvpos);
			if (hp > 0) { enemy3->setHp(static_cast<uint16_t>(hp)); }
			if (attack > 0) { enemy3->setAttack(static_cast<uint16_t>(attack)); }
			hp = 0;
			attack=0;
		}*/ else if (currentType == "player")
		{
			playerpos = csvpos;
			if (hp > 0){ player->setHp(static_cast<uint16_t>(hp)); }
			if (attack > 0) { player->setAttack(static_cast<uint16_t>(attack)); }
			hp = 0;
			attack = 0;
		}
	}
	//player->setPos(playerpos);
	player->getAttack();
	player->getHp();
	enemy->getAttack();
	enemy->getHp();

	sound = new Sound("Resources/SE/practiseBGM.wav");
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	player->mycoll.group.emplace_front(player->createCollider());
	enemy->mycoll.group.emplace_front(enemy->createCollider());

	Sound::SoundPlayWave(sound, XAUDIO2_LOOP_INFINITE);
	
}

void GameMainScene::update()
{
	// 拍内進行度と拍数を更新
	nowBeatRaito = Timer::calcNowBeatRaito((float)timer->getNowTime(), bpm, nowCount);

	if (enemy->attackFlag == true)

	{
		CollisionMgr::checkHitAll(enemy->atkcoll, player->mycoll);
		cameraObj->update();
		player->update();
		enemy->update();
		StageObj->update();
	}

	if (player->attackFlag == true)
	{
		CollisionMgr::checkHitAll(player->atkcoll, enemy->mycoll);
	}

	{
		const float raitoColor = std::lerp(0.25f, 1.f, 1.f - nowBeatRaito);
		player->setCol(XMFLOAT4(raitoColor, raitoColor, raitoColor, 1.f));

		const bool judgeRet = Timer::judge(nowBeatRaito, judgeOkRange);
		player->setJudge(judgeRet);
	}
	
	if (hpbar < 0)
	{
		hpbar = player->getHp();
	}

	cameraObj->update();
	if (hpbar > 0)
	{
		hpbar -= 0.1f;

		//hpBar->setSize(XMFLOAT2(64 * hpbar, 360));
	}
	
}

void GameMainScene::drawObj3d()
{
	groundObj->drawWithUpdate(light.get());

	//spCom->drawStart(DX12Base::getInstance()->getCmdList());
	//titleBack->drawWithUpdate(DX12Base::ins(), spCom.get());
	//hpBar->drawWithUpdate(DX12Base::ins(), spCom.get());
	StageObj->drawWithUpdate(light.get());
	player->drawWithUpdate(light.get());
	if (enemy->getAlive())
	{
		enemy->drawWithUpdate(light.get());
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
		enemy->drawWithUpdate(light.get());
	
	

	ImGui::End();
}
