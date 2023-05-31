#include "GameMainScene.h"
#include "GameOverScene.h"
#include "GameClearScene.h"
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
	{
		PlayerModel = std::make_unique<ObjModel>("Resources/cube/", "cube");
		player = std::make_unique<Player>(cameraobj.get(), PlayerModel.get());
		player->setHp(20u);
	}
	
	// --------------------
	//エネミー
	// --------------------
	{
		EnemyModel = std::make_unique<ObjModel>("Resources/enemy/", "enemy");
		enemy = std::make_unique<BaseEnemy>(cameraobj.get(), EnemyModel.get());

		enemy->setHp(20u);
		enemy->setTargetObj(player.get());
		enemy->setPos({ 20,0,0 });
	}
	
	light.reset(new Light());
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);
	//コリジョン生成
	player->Mycoll.group.emplace_front(player->createCollider());
	enemy->Mycoll.group.emplace_front(enemy->createCollider());
	
	
}

void GameMainScene::update()
{
	//各アップデート
	{
		enemy->setTargetObj(player.get());
		cameraobj->update();
		player->update();
		player->AtkObj->update();
		enemy->update();
		enemy->AtkObj->update();
		
		
	}
	//カメラ設定
	{
		Playerpos = player->getPos();
		cameraobj->setEye({ Playerpos.x, Playerpos.y + 25, Playerpos.z - 30 });
		cameraobj->setTarget(Playerpos);

	}
	//コリジョン判定
	{
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

	if (!player->getAlive())
	{
		SceneManager::getInstange()->changeScene<GameOverScene>();
	}

	
	//if (input->triggerKey(DIK_SPACE) ||
	//		input->triggerPadButton(Input::PAD::A) ||
	//		input->triggerPadButton(Input::PAD::B))
	//{
	//	SceneManager::getInstange()->changeScene<TitleScene>();
	//}


}

void GameMainScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());
	titleBack->drawWithUpdate(DX12Base::ins(), spCom.get());
	//draw
	{
		if (player->getAlive())
		{
			player->drawWithUpdate(light.get());
			player->AtkObj->drawWithUpdate(light.get());
			player->particlMgr->drawWithUpdate();
		}

		if (enemy->getAlive())
		{
			enemy->drawWithUpdate(light.get());
			enemy->AtkObj->drawWithUpdate(light.get());
			enemy->particlMgr->drawWithUpdate();
		}

	}

}
