#include "EnemyMgr.h"

#include "BaseEnemy.h"
#include <3D/Light/Light.h>
#include <Camera/Camera.h>
#include <3D/Obj/ObjModel.h>

std::weak_ptr<BaseEnemy> EnemyMgr::addEnemy(Camera* camera,
											ObjModel* model,
											const EnemyMgr::EnemyParam& param)
{
	return enemys.emplace_back(std::make_shared<BaseEnemy>(camera, model));
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
