#include "GameMainScene.h"
#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"

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
	PlayerModel.reset(new ObjModel("Resources/cube/", "cube"));
	//PlayerObj.reset(new GameObj(cameraobj.get(), PlayerModel.get()));
	player.reset(new Player(cameraobj.get(), PlayerModel.get()));

	light.reset(new Light());
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);
}

void GameMainScene::update()
{
	cameraobj->setEye({ 0, 0, 0 });
	if (input->triggerKey(DIK_SPACE) ||
			input->triggerPadButton(Input::PAD::A) ||
			input->triggerPadButton(Input::PAD::B))
	{
		SceneManager::getInstange()->changeScene<TitleScene>();
	}
	//PlayerObj->update();
	player->update();
	Playerpos = player->getPos();
}

void GameMainScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());
	titleBack->drawWithUpdate(DX12Base::ins(), spCom.get());
	player->drawWithUpdate(light.get());
	//PlayerObj->drawWithUpdate(light.get());

}
