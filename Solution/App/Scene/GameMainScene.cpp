#include "GameMainScene.h"
#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"
#include <CollisionMgr.h>

using namespace DirectX;

GameMainScene::GameMainScene()
{
	PostEffect::getInstance()->setAlpha(1.f);
	PostEffect::getInstance()->setMosaicNum(DirectX::XMFLOAT2(WinAPI::window_width, WinAPI::window_height));

	input = Input::getInstance();

	spCom.reset(new SpriteBase());

	titleBack = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/titleBack.png"),
										 spCom.get(),
										 XMFLOAT2(0.f, 0.f));
	titleBack->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));
	// デバッグテキスト用のテクスチャ読み込み
	debugText.reset(new DebugText(spCom->loadTexture(L"Resources/debugfont.png"), spCom.get()));

	cameraobj.reset(new CameraObj(player.get()));
	// --------------------
	//プレイヤー
	// --------------------

	PlayerModel = std::make_unique<ObjModel>("Resources/cube/", "cube");
	player = std::make_unique<Player>(cameraobj.get(), PlayerModel.get());
	player->setHp(20u);
	hpBar = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/hp.png"),
									 spCom.get(),
							 XMFLOAT2(0.f, 0.f));
	hpBar->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));

	// --------------------
	//エネミー
	// --------------------
	EnemyModel = std::make_unique<ObjModel>("Resources/enemy/", "enemy");
	enemy = std::make_unique<BaseEnemy>(cameraobj.get(), EnemyModel.get());

	enemy->setHp(2u);
	light.reset(new Light());

	const std::vector<std::string> fileNames = { "Resources/Csv/enemy.csv","Resources/Csv/enemy2.csv","Resources/Csv/enemy3.csv","Resources/Csv/player.csv" };
	Util::CSVType csvData = Util::loadCsv(fileNames, true, ',', "//");
	XMFLOAT3 csvpos{};
	std::vector<XMFLOAT3> enemypos;
	std::vector<XMFLOAT3> enemypos2;
	std::vector<XMFLOAT3> enemypos3;
	std::vector<XMFLOAT3> playerpos;
	std::string currentType;
	float hp = 0.0f;

	for (size_t i = 0; i < csvData.size(); i++)
	{
		hp = 0;
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
			continue;
		}

		if (currentType == "enemy")
		{
			enemypos.push_back(csvpos);
			enemy->setHp(static_cast<uint16_t>(hp));
		} else if (currentType == "enemy2")
		{
			enemypos2.push_back(csvpos);
		} else if (currentType == "enemy3")
		{
			enemypos3.push_back(csvpos);
		} else if (currentType == "player")
		{
			playerpos.push_back(csvpos);
			player->setHp(static_cast<uint16_t>(hp));
		}
	}

}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	player->Mycoll.group.emplace_front(player->createCollider());
	enemy->Mycoll.group.emplace_front(enemy->createCollider());
	
	
}

void GameMainScene::update()
{
	{
		cameraobj->update();
		player->update();
		player->AtkObj->update();
		enemy->update();
		enemy->AtkObj->update();
	}
	
	Playerpos = player->getPos();

	cameraobj->setEye({ Playerpos.x, Playerpos.y + 25, Playerpos.z - 30 });
	cameraobj->setTarget(Playerpos);
	//if (input->triggerKey(DIK_SPACE) ||
	//		input->triggerPadButton(Input::PAD::A) ||
	//		input->triggerPadButton(Input::PAD::B))
	//{
	//	SceneManager::getInstange()->changeScene<TitleScene>();
	//}
	
	if (enemy->AttackFlag == true)
	{
		CollisionMgr::checkHitAll(enemy->Atkcoll, player->Mycoll);
	}

	if (player->AttackFlag == true)
	{
		player->AtkObj->setCol({ 0,1,0,1 });
		CollisionMgr::checkHitAll(player->Atkcoll, enemy->Mycoll);

	}

	

}

void GameMainScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());
	titleBack->drawWithUpdate(DX12Base::ins(), spCom.get());
	hpBar->drawWithUpdate(DX12Base::ins(), spCom.get());
	/*if (player->getAlive())
	{
		player->drawWithUpdate(light.get());
	}*/
	player->drawWithUpdate(light.get());
	player->AtkObj->drawWithUpdate(light.get());
	if (enemy->getAlive())
	{
		enemy->drawWithUpdate(light.get());
		enemy->AtkObj->drawWithUpdate(light.get());
	}


}
