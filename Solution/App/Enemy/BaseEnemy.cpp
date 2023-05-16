#include "BaseEnemy.h"
#include <random>
BaseEnemy::BaseEnemy(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos)
{
	this->setPos({ 20,0,0 });
	AtkObj->setPos(this->getPos());
}

void BaseEnemy::update()
{
	ActTime++;
	//Move();
	this->setCol({ 1,0,0,1 });

	Attack();
}

void BaseEnemy::Move()
{

	if (moved == false)
	{

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> random_posX(minX, maxX);
		std::uniform_real_distribution<float> random_posZ(minZ, maxZ);
		dir = { random_posX(gen) ,0.0f,random_posZ(gen) };
		waitTime = 0;
		moved = true;

	} else if (moved == true)
	{
		waitTime++;
		if (waitTime >= 100)
		{
			moved = false;
		}
	}
	MoveProcess(dir);
}

void BaseEnemy::Attack()
{
	AttackFlag = true;
	AttackProcess();
	this->setCol({ 0,0,0,1 });
}
