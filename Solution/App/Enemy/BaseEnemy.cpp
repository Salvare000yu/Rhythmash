#include "BaseEnemy.h"
#include <random>
BaseEnemy::BaseEnemy(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos), enemyBehavior(std::make_unique<EnemyBehavior>(this))
{
	this->setPos({ 20,0,0 });
	AtkObj->setPos({ obj->position.x ,obj->position.y,obj->position.z + 5 });
	//setPhase([&] { return enemyBehavior->run(); });

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
	dir = { -vec.x, 0, -vec.z };

	if (TargetFromDistance() < 0.5f)
	{
		movestop = true;
	}
	if (movestop == true)
	{
		dir = { 0, 0, 0 };
		waitTime++;
		if (waitTime > 50)
		{
			movestop = false;
			waitTime = 0;
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
		waitTime = 0;
		movestop = true;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> random_posX(minX, maxX);
		std::uniform_real_distribution<float> random_posZ(minZ, maxZ);
		dir = { random_posX(gen) ,0.0f,random_posZ(gen) };
	}

	else if (movestop == true)
	{
		waitTime++;
		if (waitTime >= 100)
		{
			movestop = false;
		}
	}


}

void BaseEnemy::Attack()
{
	if (AttackFlag == false)
	{
		AttackFlag = true;
	}
	AttackProcess();
	this->setCol({ 0,0,0,1 });
}
