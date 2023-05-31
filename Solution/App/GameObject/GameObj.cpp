#include "GameObj.h"

using namespace DirectX;

void GameObj::additionalUpdate()
{
	for (auto& i : additionalUpdateProc)
	{
		i.second();
	}
}

void GameObj::additionalDraw(Light* light)
{
	for (auto& i : additionalDrawProc)
	{
		i.second(light);
	}
}

bool GameObj::damage(uint16_t damegeNum, bool killFlag)
{
	if (damegeNum >= hp)
	{
		hp = 0u;
		if (killFlag) { kill(); }
		return true;
	}

	hp -= damegeNum;
	return false;
}

XMFLOAT3 GameObj::move(const DirectX::XMVECTOR& dirNormal, float speed, bool moveYFlag, bool moveFlag)
{
	XMVECTOR velVec = XMVector3Transform(dirNormal, obj->getMatRota());

	// Y方向に移動しないならY成分を消す
	if (!moveYFlag)
	{
		velVec = XMVector3Normalize(XMVectorSetY(velVec, 0.f));
	}

	// absがあるのは、大きさのみ指定したいから。
	// absがないと、moveVelがマイナスの場合に
	// マイナス * マイナスでプラスになってしまう
	velVec *= std::abs(speed);

	XMFLOAT3 vel{};
	XMStoreFloat3(&vel, velVec);

	if(moveFlag)
	{
		obj->position.x += vel.x;
		obj->position.y += vel.y;
		obj->position.z += vel.z;
	}

	return vel;
}

void GameObj::moveForward(float moveVel, bool moveYFlag)
{
	move(XMVectorSet(0, 0, 1, 0), moveVel, moveYFlag);
}

void GameObj::moveRight(float moveVel, bool moveYFlag)
{
	move(XMVectorSet(1, 0, 0, 0), moveVel, moveYFlag);
}

void GameObj::moveParentRight(float moveVel, bool moveYFlag)
{
	if (!obj->parent)
	{
		moveRight(moveVel, moveYFlag);
		return;
	}

	// X方向のベクトルを、自機の向いている向きに回転
	XMVECTOR velVec = XMVector3Transform(XMVectorSet(moveVel, 0, 0, 1), obj->parent->getMatRota());

	// Y方向に移動しないならY成分を消す
	if (!moveYFlag)
	{
		// absがあるのは、大きさのみ指定したいから。
		// absがないと、moveVelがマイナスの場合に
		// マイナス * マイナスでプラスになってしまう
		velVec = XMVectorScale(XMVector3Normalize(XMVectorSetY(velVec, 0.f)),
							   std::abs(moveVel));
	}

	obj->position.x += XMVectorGetX(velVec);
	obj->position.y += XMVectorGetY(velVec);
	obj->position.z += XMVectorGetZ(velVec);
}

void GameObj::moveUp(float moveVel)
{
	// Y方向のベクトルを、自機の向いている向きに回転
	XMVECTOR velVec = XMVector3Transform(XMVectorSet(0, moveVel, 0, 1), obj->getMatRota());

	obj->position.x += XMVectorGetX(velVec);
	obj->position.y += XMVectorGetY(velVec);
	obj->position.z += XMVectorGetZ(velVec);
}

void GameObj::moveParentUp(float moveVel)
{
	if (!obj->parent)
	{
		moveUp(moveVel);
		return;
	}

	// Y方向のベクトルを、自機の向いている向きに回転
	XMVECTOR velVec = XMVector3Transform(XMVectorSet(0, moveVel, 0, 1), obj->parent->getMatRota());

	obj->position.x += XMVectorGetX(velVec);
	obj->position.y += XMVectorGetY(velVec);
	obj->position.z += XMVectorGetZ(velVec);
}

GameObj::GameObj(Camera* camera,
				 ObjModel* model,
				 const DirectX::XMFLOAT3& pos)
	: obj(std::make_unique<Object3d>(camera, model)),
	ppStateNum(Object3d::ppStateNum)
{
	setPos(pos);
}

GameObj::~GameObj()
{
	obj.reset(nullptr);
}

void GameObj::update()
{
	obj->update();
	for (auto& i : otherObj)
	{
		i.second->update();
	}
	additionalUpdate();
}

void GameObj::draw(Light* light)
{
	obj->draw(light, ppStateNum);
	for (auto& i : otherObj)
	{
		i.second->draw(light);
	}
	additionalDraw(light);
}

void GameObj::drawWithUpdate(Light* light)
{
	update();

	draw(light);
}