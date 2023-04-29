#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <array>
#include <System/DX12Base.h>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Light
{
public:
	// 平行光源の最大数
	// シェーダー側と合わせる
	constexpr static unsigned DirLightCountMax = 3u;

	// 点光源の最大数
	// シェーダー側と合わせる
	constexpr static unsigned PointLightCountMax = 3u;

	// スポットライトの最大数
	// シェーダー側と合わせる
	constexpr static unsigned SpotLightCountMax = 3u;

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct ConstBufferData
	{
		DirectX::XMFLOAT3 ambientColor{ 1, 1, 1 };	// アンビエント色
		unsigned activeDirLightCount;	// 有効な平行光源の数
		unsigned activePointLightCount;	// 有効な点光源の数
		unsigned activeSpotLightCount;	// 有効なスポットライトの数
		DirectionalLight::ConstBufferData dirLights[DirLightCountMax]{};
		PointLight::ConstBufferData pointLights[PointLightCountMax]{};
		SpotLight::ConstBufferData spotLights[SpotLightCountMax]{};
	};

private:
	static DX12Base* dxBase;

private:
	ComPtr<ID3D12Resource> constBuff;	// 定数バッファ

	DirectX::XMFLOAT3 ambientColor{ 1, 1, 1 };	// アンビエント色

	bool dirty = false;

	std::array<PointLight, PointLightCountMax> pointLights;
	std::array<DirectionalLight, DirLightCountMax> dirLights;
	std::array<SpotLight, DirLightCountMax> spotLights;

public:
	Light();

	inline void setAmbientColor(const DirectX::XMFLOAT3& color) { this->ambientColor = color; dirty = true; }
	inline const DirectX::XMFLOAT3& getAmbientColor() const { return this->ambientColor; }

#pragma region 点光源アクセッサ

	inline void setPointLightActive(unsigned ind, bool active) { pointLights[ind].setActive(active); dirty = true; }
	inline bool getPointLightActive(unsigned ind) const { return pointLights[ind].getActive(); }

	inline void setPointLightPos(unsigned ind, const DirectX::XMFLOAT3& pos) { pointLights[ind].setPos(pos); dirty = true; }
	inline const auto& getPointLightPos(unsigned ind) const { return pointLights[ind].getPos(); }

	inline void setPointLightColor(unsigned ind, const DirectX::XMFLOAT3& color) { pointLights[ind].setColor(color); dirty = true; }
	inline const auto& getPointLightColor(unsigned ind) const { return pointLights[ind].getColor(); }

	inline void setPointLightAtten(unsigned ind, const DirectX::XMFLOAT3& atten) { pointLights[ind].setAtten(atten); dirty = true; }
	inline const auto& getPointLightAtten(unsigned ind) const { return pointLights[ind].getAtten(); }

#pragma endregion 点光源アクセッサ

#pragma region 平行光源アクセッサ

	inline void setDirLightActive(unsigned ind, bool active) { dirLights[ind].setActive(active); dirty = true; }
	inline bool getDirLightActive(unsigned ind) const { return dirLights[ind].getActive(); }

	inline void setDirLightDir(unsigned ind, const DirectX::XMVECTOR& dirNormal) { dirLights[ind].setDir(dirNormal); dirty = true; }
	inline const auto& getDirLightDir(unsigned ind) const { return dirLights[ind].getDir(); }

	inline void setDirLightColor(unsigned ind, const DirectX::XMFLOAT3& color) { dirLights[ind].setColor(color); dirty = true; }
	inline const auto& getDirLightColor(unsigned ind) const { return dirLights[ind].getColor(); }

#pragma endregion 平行光源アクセッサ

#pragma region スポットライトアクセッサ

	inline void setSpotLightActive(unsigned ind, bool active) { this->spotLights[ind].setActive(active); dirty = true; }
	inline bool getSpotLightActive(unsigned ind) const { return spotLights[ind].getActive(); }

	inline void setSpotLightDir(unsigned ind, const DirectX::XMVECTOR& dir) { this->spotLights[ind].dir = dir; dirty = true; }
	inline const auto& getSpotLightDir(unsigned ind) const { return spotLights[ind].dir; }

	inline void setSpotLightPos(unsigned ind, const DirectX::XMFLOAT3& pos) { this->spotLights[ind].pos = pos; dirty = true; }
	inline const auto& getSpotLightPos(unsigned ind) const { return spotLights[ind].pos; }

	inline void setSpotLightColor(unsigned ind, const DirectX::XMFLOAT3& color) { this->spotLights[ind].color = color; dirty = true; }
	inline const auto& getSpotLightColor(unsigned ind) const { return spotLights[ind].color; }

	inline void setSpotLightAtten(unsigned ind, const DirectX::XMFLOAT3& atten) { this->spotLights[ind].atten = atten; dirty = true; }
	inline const auto& getSpotLightAtten(unsigned ind) const { return spotLights[ind].atten; }

	inline void setSpotLightFactorAngleCos(unsigned ind, const DirectX::XMFLOAT2& factorAngleCos) { this->spotLights[ind].factorAngleCos = factorAngleCos; dirty = true; }
	inline const auto& getSpotLightFactorAngleCos(unsigned ind) const { return spotLights[ind].factorAngleCos; }

#pragma endregion スポットライトアクセッサ

	//定数バッファ転送
	void transferConstBuffer();

	void update();

	// @param rootParamIndex : Object3dクラスのcreateGraphicsPipeline関数内のrootParamsの要素数
	void draw(UINT rootParamIndex);
};
