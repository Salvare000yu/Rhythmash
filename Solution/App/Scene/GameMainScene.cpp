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
#include "Sound/Sound.h"
#include <3D/ParticleMgr.h>

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
	light->setDirLightActive(0, true);
	light->setCircleShadowActive(0, true);

	PostEffect::getInstance()->setAlpha(1.f);
	PostEffect::getInstance()->setMosaicNum(DirectX::XMFLOAT2(WinAPI::window_width, WinAPI::window_height));

	cameraObj = std::make_unique<CameraObj>(nullptr);
	cameraObj->easeRaito = 0.5f;
	cameraObj->useParentRotaFlag = false;

	// --------------------
	// パーティクル
	// --------------------
	particleMgr = std::make_shared<ParticleMgr>(L"Resources/white.png", cameraObj.get());

	// --------------------
	// 背景
	// --------------------
	groundModel = std::make_unique<ObjModel>("Resources/ground/", "ground");
	groundObj = std::make_unique<Object3d>(cameraObj.get(), groundModel.get());
	groundObj->position.y -= 10.f;
	constexpr float groundSize = 1000.f;
	groundObj->scale = XMFLOAT3(groundSize, 1.f, groundSize);
	groundModel->setTexTilling(XMFLOAT2(groundSize, groundSize));

	groundObj->color = XMFLOAT4(1, 1, 1, 0.5f);

	// --------------------
	// 音
	// --------------------
	bgm = Sound::ins()->loadWave("Resources/SE/practiseBGM.wav");
	damageSe = Sound::ins()->loadWave("Resources/SE/damage.wav");

	// --------------------
	// 自機
	// --------------------

	playerModel = std::make_unique<ObjModel>("Resources/player_robot/", "player_robot");
	player = std::make_unique<Player>(cameraObj.get(), playerModel.get());
	player->setDamageSe(damageSe);
	player->setJudgeProc([&] { return Timer::judge(player->getNowBeatRaito(), judgeOkRange); });
	player->setCol(XMFLOAT4(1, 1, 1, 0.8f));

	playerCols.group.emplace_front(CollisionMgr::ColliderType::create(player.get(), player->getScaleF3().z));
	auto pAtk = player->getAtkObjPt().lock();
	playerAtkCols.group.emplace_front(CollisionMgr::ColliderType::create(pAtk.get(), pAtk->getScaleF3().z));

	cameraObj->setParentObj(player.get());

	// --------------------
	// 敵
	// --------------------
	enemyModel = std::make_unique<ObjModel>("Resources/enemy/", "enemy");

	stageModel = std::make_unique<ObjModel>("Resources/ring/", "ring");
	stageObj = std::make_unique<GameObj>(cameraObj.get(), stageModel.get());
	stageObj->setScale(10);

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

	// --------------------
	// コライダー衝突時関数
	// --------------------
	std::function<void(GameObj*)> hitProc = [&](GameObj* obj)
	{
		Sound::playWave(damageSe);
		particleMgr->createParticle(obj->calcWorldPos(), 50ui16);
		if (obj->damage(1ui16, true))
		{
			obj->setCol(XMFLOAT4(0, 0, 0, 1));
			return;
		}
		obj->setCol({ 1,0,0,obj->getCol().w });
	};

	playerCols.hitProc = hitProc;
	enemyCols.hitProc = hitProc;
}

GameMainScene::~GameMainScene()
{
	Sound::stopWave(bgm);
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	Sound::playWave(bgm, XAUDIO2_LOOP_INFINITE);

	// タイマーの起点時間をリセット
	timer->reset();
}

void GameMainScene::update()
{
	// 拍内進行度と拍数を更新
	nowBeatRaito = Timer::calcNowBeatRaito((float)timer->getNowTime(), bpm, nowCount);

	// 敵コライダーを初期化
	enemyCols.group.clear();
	enemyAtkCols.group.clear();
	for (auto& i : enemy)
	{
		if (i->getAlive())
		{
			enemyCols.group.emplace_front(CollisionMgr::ColliderType::create(i.get(), i->getScaleF3().z));
		}

		if (i->getAttackFlag())
		{
			auto atk = i->getAtkObjPt().lock();
			enemyAtkCols.group.emplace_front(CollisionMgr::ColliderType::create(atk.get(), atk->getScaleF3().z));
		}
	}

	// 当たり判定をする
	if (player->getAttackFlag())
	{
		CollisionMgr::checkHitAll(playerAtkCols, enemyCols);
	}
	CollisionMgr::checkHitAll(enemyAtkCols, playerCols);

	// 死んだ敵は消す
	std::erase_if(enemy, [](const std::shared_ptr<BaseEnemy>& e) { return !e->getAlive(); });

	// 拍情報更新
	{
		const float raitoColor = std::lerp(0.25f, 1.f, 1.f - nowBeatRaito);
		player->setCol(XMFLOAT4(raitoColor, raitoColor, raitoColor, player->getCol().w));

		player->setNowBeatRaito(nowBeatRaito);
		for (auto& i : enemy)
		{
			i->setNowBeatRaito(nowBeatRaito);
			i->setNowBeatCount((uint32_t)nowCount);
		}
	}

	// シーン移行
	if (!player->getAlive())
	{
		SceneManager::getInstange()->changeScene<GameOverScene>();
	} else if (enemy.empty())
	{
		SceneManager::getInstange()->changeScene<GameClearScene>();
	}

	groundObj->update();
	stageObj->update();
	{
		light->setCircleShadowActiveAll(false);

		player->update();
		light->setCircleShadowActive(0, true);
		light->setCircleShadowCasterPos(0, player->calcWorldPos());
		for (unsigned i = 0u, len = (unsigned)enemy.size(); i < len; ++i)
		{
			if (!enemy[i]->getAlive()) { continue; }
			enemy[i]->update();
			light->setCircleShadowActive(1 + i, true);
			light->setCircleShadowCasterPos(1 + i, enemy[i]->calcWorldPos());
		}
	}

	cameraObj->update();
	light->update();
}

void GameMainScene::drawObj3d()
{
	groundObj->draw(light.get());

	stageObj->draw(light.get());
	player->draw(light.get());
	for (auto& i : enemy)
	{
		if (!i->getAlive()) { continue; }
		i->draw(light.get());
	}
	particleMgr->drawWithUpdate();
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
	ImGui::Text("自機体力: %u", player->getHp());

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(100, 100));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
}

std::weak_ptr<BaseEnemy> GameMainScene::addEnemy(const DirectX::XMFLOAT3& pos)
{
	auto& i = enemy.emplace_back(std::make_shared<BaseEnemy>(cameraObj.get(), enemyModel.get()));

	i->setHp(3u);
	i->setTargetObj(player.get());
	i->setPos(pos);
	i->setDamageSe(damageSe);

	const auto enemyNum = (unsigned)enemy.size();
	if (enemyNum < Light::CircleShadowCountMax)
	{
		light->setCircleShadowActive(enemyNum, true);
		light->setCircleShadowCaster2LightDistance(enemyNum, 50.f);
	}

	return i;
}