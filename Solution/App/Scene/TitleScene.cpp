#include "TitleScene.h"

#include "GameOverScene.h"
#include "System/SceneManager.h"
#include <DirectXMath.h>
#include <Util/Util.h>
#include "GameMainScene.h"

using namespace DirectX;

TitleScene::TitleScene() :
	update_proc(std::bind(&TitleScene::update_normal, this)),
	input(Input::ins())
{
	shortBridge = Sound::ins()->loadWave("Resources/SE/Shortbridge29-1.wav");

	spCom.reset(new SpriteBase());

	// デバッグテキスト用のテクスチャ読み込み
	debugText.reset(new DebugText(spCom->loadTexture(L"Resources/debugfont.png"),
								  spCom.get()));

	auto titlePos = XMFLOAT2(0.f, 0.f);

	titleLogo_back = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/logo_back.png"),
										 spCom.get(),
										 titlePos);
	titleLogo_back->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));

	titleLogo = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/logo_title.png"),
										 spCom.get(),
										 titlePos);
	titleLogo->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));

	
	titleLogo_rubi = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/logo_rubi.png"),
										 spCom.get(),
										 titlePos);
	titleLogo_rubi->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));

	titleBack = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/titleBack.png"),
										 spCom.get(),
										 XMFLOAT2(0.f, 0.f));

	titlePressKey = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/title_PressKey.png"),
										 spCom.get(),
										 XMFLOAT2(0.f, 0.f));
	titleBack->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));
}

void TitleScene::start()
{
	// 次シーンの読み込み開始
	sceneThread.reset(new MyThread());
	sceneThread->thread.reset(new std::thread([&] { nextScene = std::make_unique<GameMainScene>(); }));

	//Sound::SoundPlayWave(bgm.get(), XAUDIO2_LOOP_INFINITE, 0.2f);
}

TitleScene::~TitleScene()
{}

void TitleScene::update()
{
	update_proc();
}

void TitleScene::update_normal()
{
	if (input->triggerKey(DIK_0))
	{
		//Sound::SoundStopWave(bgm.get());
		sceneThread->join();
		nextScene.reset(new GameOverScene());
		update_proc = std::bind(&TitleScene::update_end, this);
	} else if (input->triggerKey(DIK_SPACE) ||
		input->triggerPadButton(Input::PAD::A) ||
		input->triggerPadButton(Input::PAD::B))
	{
		update_proc = std::bind(&TitleScene::update_end, this);
		//Sound::SoundPlayWave(shortBridge.get());
		//Sound::SoundStopWave(bgm.get());
	}
}

void TitleScene::update_end()
{
	// 次シーンの読み込み終了を待つ
	sceneThread->join();
	// 次シーンへ進む
	SceneManager::getInstange()->changeSceneFromInstance(nextScene);
}

void TitleScene::drawFrontSprite()
{
	spCom->drawStart(DX12Base::ins()->getCmdList());
	titleBack->drawWithUpdate(DX12Base::ins(), spCom.get());
	titleLogo_back->drawWithUpdate(DX12Base::ins(), spCom.get());
	titleLogo->drawWithUpdate(DX12Base::ins(), spCom.get());
	titleLogo_rubi->drawWithUpdate(DX12Base::ins(), spCom.get());
	titlePressKey->drawWithUpdate(DX12Base::ins(), spCom.get());
}