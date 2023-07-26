#include "EnemyMgr.h"

#include "BaseEnemy.h"
#include <3D/Light/Light.h>
#include <Camera/Camera.h>
#include <3D/Obj/ObjModel.h>
#include <Util/Timer.h>

std::weak_ptr<BaseEnemy> EnemyMgr::addEnemy(Camera* camera,
											ObjModel* model,
											std::weak_ptr<Timer> timer)
{
	return enemys.emplace_back(std::make_shared<BaseEnemy>(camera, model, timer));
}

void EnemyMgr::update()
{
	for (auto& i : enemys)
	{
		i->update();
	}
}

void EnemyMgr::draw(Light* light)
{
	for (auto& i : enemys)
	{
		i->draw(light);
	}
}