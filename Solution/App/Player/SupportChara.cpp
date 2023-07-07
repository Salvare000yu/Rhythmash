#include "SupportChara.h"
using namespace DirectX;

SupportChara::SupportChara(Camera* camera, ObjModel* model, const DirectX::XMFLOAT3& pos)
	:BaseActObj(camera, model, pos)
{

}

void SupportChara::Move()
{
	dir = { 0,0,0 };

	if (judge())
	{
		Step();
	}

	MoveProcess(dir);

}

void SupportChara::Step()
{

	if (Rev == true)
	{
		dir.y = 1;
		RevCount += 1;
	}

	else if (Rev == false)
	{
		dir.y = -1;
		RevCount += 1;
	}

	if (RevCount >= 3)
	{
		if (Rev == true)
		{
			Rev = false;
		}

		else if (Rev == false)
		{
			Rev = true;
		}
		RevCount = 0;
	}
}

