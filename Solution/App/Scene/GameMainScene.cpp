#include "GameMainScene.h"
#include "TitleScene.h"
#include "System/PostEffect.h"
#include "System/SceneManager.h"

GameMainScene::GameMainScene()
{
	PostEffect::getInstance()->setAlpha(1.f);
	PostEffect::getInstance()->setMosaicNum(DirectX::XMFLOAT2(WinAPI::window_width, WinAPI::window_height));

	input = Input::getInstance();

	spCom.reset(new SpriteBase());

	// デバッグテキスト用のテクスチャ読み込み
	debugText.reset(new DebugText(spCom->loadTexture(L"Resources/debugfont.png"), spCom.get()));

	player.reset(new Player());
}

void GameMainScene::start()
{
	// マウスカーソルは表示する
	input->changeDispMouseCursorFlag(true);
}

void GameMainScene::update()
{
	
	if (input->triggerKey(DIK_SPACE) ||
			input->triggerPadButton(Input::PAD::A) ||
			input->triggerPadButton(Input::PAD::B))
	{
		SceneManager::getInstange()->changeScene<TitleScene>();
	}
	player->update(input,0.2f);
}

void GameMainScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::getInstance()->getCmdList());
	player->Draw();
}
