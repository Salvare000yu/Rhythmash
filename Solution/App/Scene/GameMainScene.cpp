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
	player->setAlive(true);

	// --------------------
	//エネミー
	// --------------------
	EnemyModel = std::make_unique<ObjModel>("Resources/enemy/", "enemy");
	enemy = std::make_unique<BaseEnemy>(cameraobj.get(), PlayerModel.get());

	enemy->setHp(2u);
	light.reset(new Light());


}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);

	player->mycoll.group.emplace_front(player->createCollider());
	enemy->mycoll.group.emplace_front(enemy->createCollider());
}

void GameMainScene::update()
{

	cameraobj->update();
	Playerpos = player->getPos();

	cameraobj->setEye({ Playerpos.x, Playerpos.y + 25, Playerpos.z - 30 });
	cameraobj->setTarget(Playerpos);
	//if (input->triggerKey(DIK_SPACE) ||
	//		input->triggerPadButton(Input::PAD::A) ||
	//		input->triggerPadButton(Input::PAD::B))
	//{
	//	SceneManager::getInstange()->changeScene<TitleScene>();
	//}

	player->update();

	enemy->update();
	enemy->setPos({ 50,0,0 });
	player->setCol({ 1,1,1,1 });
	
	if (player->getAlive())
	{
		CollisionMgr::checkHitAll(enemy->mycoll, player->mycoll);
	}
	if (!player->getAlive())
	{
		player->setCol({ 0,0,0,1 });
	}

}

void GameMainScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());
	titleBack->drawWithUpdate(DX12Base::ins(), spCom.get());
	
	if (player->getAlive())
	{
		player->drawWithUpdate(light.get());
	}


	if (enemy->getAlive())
	{
		enemy->drawWithUpdate(light.get());
	}


}
