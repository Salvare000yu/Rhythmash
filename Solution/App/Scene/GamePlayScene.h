#pragma once
#include <System/GameScene.h>
#include <memory>
#include <unordered_map>
#include <string>
#include "Util/Util.h"

class GameObj;
class Light;
class CameraObj;
class Object3d;
class ObjModel;

class GamePlayScene :
    public GameScene
{
private:

	size_t backState;
	std::unique_ptr<Object3d> groundObj;
	std::unique_ptr<ObjModel> groundModel;

	std::unordered_map<std::string, std::unique_ptr<GameObj>> objs;
	std::unordered_map<std::string, std::unique_ptr<ObjModel>> models;
	std::unique_ptr<Light> light;
	std::unique_ptr<CameraObj> cameraObj;

public:
	GamePlayScene();
	~GamePlayScene();

	void update() override;
	void drawObj3d() override;
	void drawFrontSprite() override;
};

