#include "BaseEnemy.h"
#include <random>
BaseEnemy::BaseEnemy(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos), enemyBehavior(std::make_unique<EnemyBehavior>(this))
{
	this->setPos({ 20,0,0 });
	AtkObj->setPos({ obj->position.x ,obj->position.y,obj->position.z + 5 });
}

float BaseEnemy::TargetFromDistance()
{

	if (!this->targetObj) { return -1.f; }
	const XMVECTOR pos = XMLoadFloat3(&this->calcWorldPos());
	const XMVECTOR tpos = XMLoadFloat3(&this->targetObj->calcWorldPos());

	return Collision::vecLength(pos - tpos);
}

void BaseEnemy::update()
{
	if (hp <= 0)
	{
		kill();
		AtkObj->kill();
	}
	this->setCol({ 1,0,0,1 });
	enemyBehavior->run();
}

void BaseEnemy::MovetoTarget()
{
	pos = this->getPos();
	tpos = this->targetObj->calcWorldPos();
	XMFLOAT3 vec = { pos.x - tpos.x, 0, pos.z - tpos.z };
	dir = { -vec.x , 0, -vec.z };

	if (TargetFromDistance() < 3.0f)
	{
		movestop = true;
	}
	if (movestop == true)
	{
		dir = { 0, 0, 0 };
		MoveWaitTime++;
		if (MoveWaitTime > 50)
		{
			movestop = false;
			MoveWaitTime = 0;
		}
	}

}

void BaseEnemy::Move()
{

	if (TargetFromDistance() < 20.0f)
	{
		MovetoTarget();
	} else
	{
		RandomMove();
	}
	MoveProcess(dir);
}

void BaseEnemy::RandomMove()
{

	if (movestop == false)
	{
		MoveWaitTime = 0;
		movestop = true;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> random_posX(minX, maxX);
		std::uniform_real_distribution<float> random_posZ(minZ, maxZ);
		dir = { random_posX(gen) ,0.0f,random_posZ(gen) };
	}

	else if (movestop == true)
	{
		MoveWaitTime++;
		if (MoveWaitTime >= 100)
		{
			movestop = false;
		}
	}


}

void BaseEnemy::Attack()
{
	if (AtkWaitTime > 20)
	{
		AttackFlag = true;
		AtkWaitTime = 0;
	}
	if (AttackFlag == true)
	{
		AttackTime++;
		if (AttackTime >= 2)
		{
			AttackFlag = false;
			AtkObj->setCol({ 1,1,1,1 });
			AttackTime = 0;
		}
	}
	AtkWaitTime++;
	this->setCol({ 0,0,0,1 });
}
