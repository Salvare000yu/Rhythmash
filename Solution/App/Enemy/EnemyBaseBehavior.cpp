#include "EnemyBaseBehavior.h"
#include "BaseEnemy.h"

EnemyBaseBehavior::EnemyBaseBehavior(BaseEnemy* enemy) :
	Sequencer(), enemy(enemy), mainPhase(std::make_unique<Sequencer>())
{
	addChild(Task([&]
				  {
					  targetDistance = this->enemy->targetFromDistance();

					  // メインの行動をし、結果を取得
					  const NODE_RESULT ret = mainPhase->run();

					  // 拍数を更新
					  preBeatCount = this->enemy->getNowBeatCount();

					  // 行動の結果を返す
					  return ret;
				  }));
}