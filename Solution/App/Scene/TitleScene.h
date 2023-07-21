/*****************************************************************//**
 * \file   TitleScene.h
 * \brief  タイトル画面
 *********************************************************************/

#pragma once
#include "System/GameScene.h"

#include <memory>
#include <functional>
#include <DirectXMath.h>

#include "2D/DebugText.h"
#include "Input/Input.h"
#include "Sound/SoundData.h"

#include "Util/Timer.h"

#include <thread>
#include <forward_list>

 /// @brief タイトル画面シーンのクラス
class TitleScene :
	public GameScene
{
	// --------------------
	// 音
	// --------------------
	std::weak_ptr<SoundData> shortBridge;
	std::weak_ptr<SoundData> bgm;

	// --------------------
	// スプライト
	// --------------------
	std::unique_ptr<SpriteBase> spCom;
	std::forward_list<std::unique_ptr<Sprite>> titles;

	// --------------------
	// デバッグテキスト
	// --------------------
	std::unique_ptr<DebugText> debugText;

	// --------------------
	// シーン
	// --------------------
	struct MyThread
	{
		std::unique_ptr<std::thread> thread;

		inline void join()
		{
			if (thread && thread->joinable())
			{
				thread->join();
			}
		}

		~MyThread()
		{
			join();
		}
	};
	std::unique_ptr<MyThread> sceneThread;
	std::unique_ptr<GameScene> nextScene;

	Input* input = nullptr;

	// update_何とか関数を格納する
	std::function<void()> update_proc;

	void update_normal();
	void update_end();

public:
	TitleScene();
	~TitleScene();
	void start() override;
	void update() override;
	void drawFrontSprite() override;
};