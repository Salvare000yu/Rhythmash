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

	constexpr auto titlePos = XMFLOAT2(0.f, 0.f);

	titles.emplace_front(std::make_unique<Sprite>(spCom->loadTexture(L"Resources/title_PressKey.png"), spCom.get(), XMFLOAT2(0.f, 0.f)));
	auto& titleLogo_rubi = titles.emplace_front(std::make_unique<Sprite>(spCom->loadTexture(L"Resources/logo_rubi.png"), spCom.get(), XMFLOAT2(0.f, 0.f)));
	auto& titleLogo = titles.emplace_front(std::make_unique<Sprite>(spCom->loadTexture(L"Resources/logo_title.png"), spCom.get(), XMFLOAT2(0.f, 0.f)));
	titles.emplace_front(std::make_unique<Sprite>(spCom->loadTexture(L"Resources/logo_back.png"), spCom.get(), XMFLOAT2(0.f, 0.f)));
	auto& titleBack = titles.emplace_front(std::make_unique<Sprite>(spCom->loadTexture(L"Resources/titleBack.png"), spCom.get(), XMFLOAT2(0.f, 0.f)));

	titleLogo->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));
	titleLogo_rubi->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));
	titleBack->setSize(XMFLOAT2((float)WinAPI::window_width, (float)WinAPI::window_height));
}

void TitleScene::start()
{
	// 次シーンの読み込み開始
	sceneThread.reset(new MyThread());
	sceneThread->thread.reset(new std::thread([&] { nextScene = std::make_unique<GameMainScene>(); }));
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
		sceneThread->join();
		nextScene.reset(new GameOverScene());
		update_proc = std::bind(&TitleScene::update_end, this);
	} else if (input->triggerKey(DIK_SPACE) ||
		input->triggerPadButton(Input::PAD::A) ||
		input->triggerPadButton(Input::PAD::B))
	{
		update_proc = std::bind(&TitleScene::update_end, this);
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
	for (auto& i : titles)
	{
		i->drawWithUpdate(DX12Base::ins(), spCom.get());
	}
}