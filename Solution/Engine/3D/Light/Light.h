#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <array>
#include <System/DX12Base.h>
#include "PointLight.h"

class Light
{
public:
	// ポイントライトの最大数
	// シェーダー側と合わせる
	constexpr static unsigned PointLightCountMax = 3u;

private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct ConstBufferData
	{
		DirectX::XMFLOAT3 ambientColor{ 1, 1, 1 };	// アンビエント色
		unsigned activePointLightCount;				// 有効なライトの数
		PointLight::ConstBufferData pointLights[PointLightCountMax]{};
	};

private:
	static DX12Base* dxBase;

private:
	ComPtr<ID3D12Resource> constBuff;	// 定数バッファ

	DirectX::XMFLOAT3 ambientColor{ 1, 1, 1 };	// アンビエント色

	bool dirty = false;

	std::array<PointLight, PointLightCountMax> pointLights;

public:
	Light();

	inline void setAmbientColor(const DirectX::XMFLOAT3& color) { this->ambientColor = color; dirty = true; }
	inline const DirectX::XMFLOAT3& getAmbientColor() const { return this->ambientColor; }

	inline void setPointLightActive(unsigned ind, bool active) { pointLights[ind].setActive(active); dirty = true; }
	inline bool getPointLightActive(unsigned ind) const { return pointLights[ind].getActive(); }

	inline void setPointLightPos(unsigned ind, const DirectX::XMFLOAT3& pos) { pointLights[ind].setPos(pos); dirty = true; }
	inline const auto& getPointLightPos(unsigned ind) const { return pointLights[ind].getPos(); }

	inline void setPointLightColor(unsigned ind, const DirectX::XMFLOAT3& color) { pointLights[ind].setColor(color); dirty = true; }
	inline const auto& getPointLightColor(unsigned ind) const { return pointLights[ind].getColor(); }

	inline void setPointLightAtten(unsigned ind, const DirectX::XMFLOAT3& atten) { pointLights[ind].setAtten(atten); dirty = true; }
	inline const auto& getPointLightAtten(unsigned ind) const { return pointLights[ind].getAtten(); }

	//定数バッファ転送
	void transferConstBuffer();

	void update();

	// @param rootParamIndex : Object3dクラスのcreateGraphicsPipeline関数内のrootParamsの要素数
	void draw(UINT rootParamIndex);
};
