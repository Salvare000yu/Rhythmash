#pragma once
#include <memory>
#include <vector>

class BaseEnemy;
class Light;
class Camera;
class ObjModel;

class EnemyMgr
{
public:
	struct EnemyParam
	{
		/// @brief 体力
		uint16_t hp = 1ui16;

		/// @brief 攻撃力
		uint16_t attack = 1ui16;

		/// @brief 一回の移動で動く大きさ
		float moveVal = 10.f;
	};

private:
	/// @brief 敵リスト
	std::vector<std::shared_ptr<BaseEnemy>> enemys;

public:
	inline auto& getEnemyList() { return enemys; }

	/// @brief 敵を追加
	/// @param light ライトのポインタ
	/// @param camera カメラのポインタ
	/// @param model モデルのポインタ
	/// @param param パラメータ情報
	/// @return 追加した要素
	std::weak_ptr<BaseEnemy> addEnemy(Camera* camera,
									  ObjModel* model,
									  const EnemyParam& param);

	/// @brief 更新処理
	void update();

	/// @brief 描画処理
	void draw(Light* light);
};
