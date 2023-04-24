#include "GameClearScene.h"

#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"

GameClearScene::GameClearScene()
{
	PostEffect::getInstance()->setAlpha(1.f);
	PostEffect::getInstance()->setMosaicNum(DirectX::XMFLOAT2(WinAPI::window_width, WinAPI::window_height));

	input = Input::getInstance();

	spCom.reset(new SpriteBase());

	// デバッグテキスト用のテクスチャ読み込み
	debugText.reset(new DebugText(spCom->loadTexture(L"Resources/debugfont.png"), spCom.get()));

	clear.reset(new Sprite(spCom->loadTexture(L"Resources/GameClear/gameClear.png"),
						 spCom.get(),
						 DirectX::XMFLOAT2(0.f, 0.f)));

	clear->setSize(DirectX::XMFLOAT2((float)WinAPI::window_width,
								   (float)WinAPI::window_height));
}

void GameClearScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);
}

void GameClearScene::update()
{
	if (input->triggerKey(DIK_SPACE) ||
		input->triggerPadButton(Input::PAD::A) ||
		input->triggerPadButton(Input::PAD::B))
	{
		SceneManager::getInstange()->changeScene<TitleScene>();
	}
}

void GameClearScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());
	clear->drawWithUpdate(DX12Base::ins(), spCom.get());
}