#include "TitleScene.h"

#include "EndScene.h"
#include "GameOverScene.h"
#include "System/SceneManager.h"
#include <DirectXMath.h>
#include <Util/Util.h>

using namespace DirectX;

TitleScene::TitleScene() :
	update_proc(std::bind(&TitleScene::update_normal, this))
{
	input = Input::getInstance();

	shortBridge = std::make_unique<Sound>("Resources/SE/Shortbridge29-1.wav");
	bgm = std::make_unique<Sound>("Resources/BGM/Detour.wav");

	spCom.reset(new SpriteBase());

	// デバッグテキスト用のテクスチャ読み込み
	debugText.reset(new DebugText(spCom->loadTexture(L"Resources/debugfont.png"),
								  spCom.get()));

	auto titlePos = XMFLOAT2(0.f, 0.f);

	titleLogo = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/title_logo.png"),
										 spCom.get(),
										 titlePos);
	titleLogo->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));

	titleBack = std::make_unique<Sprite>(spCom->loadTexture(L"Resources/titleBack.png"),
										 spCom.get(),
										 XMFLOAT2(0.f, 0.f));
	titleBack->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));
}

void TitleScene::start()
{
	// 次シーンの読み込み開始
	sceneThread.reset(new MyThread());
	sceneThread->thread.reset(new std::thread([&] { nextScene = std::make_unique<EndScene>(); }));

	Sound::SoundPlayWave(bgm.get(), XAUDIO2_LOOP_INFINITE, 0.2f);
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
		Sound::SoundStopWave(bgm.get());
		sceneThread->join();
		nextScene.reset(new GameOverScene());
		update_proc = std::bind(&TitleScene::update_end, this);
	} else if (input->triggerKey(DIK_SPACE) ||
		input->triggerPadButton(Input::PAD::A) ||
		input->triggerPadButton(Input::PAD::B))
	{
		update_proc = std::bind(&TitleScene::update_end, this);
		Sound::SoundPlayWave(shortBridge.get());
		Sound::SoundStopWave(bgm.get());
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
	titleLogo->drawWithUpdate(DX12Base::ins(), spCom.get());
}