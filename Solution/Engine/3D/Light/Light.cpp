#include "Light.h"
#include <cassert>
#include <d3dx12.h>

using namespace DirectX;

DX12Base* Light::dxBase = DX12Base::getInstance();

Light::Light()
{
	//定数バッファ生成
	HRESULT result = dxBase->getDev()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	transferConstBuffer();
}

void Light::transferConstBuffer()
{
	//定数バッファへデータ転送
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->ambientColor = ambientColor;

		unsigned lightNum = 0u;
		for (auto& i : dirLights)
		{
			if (!i.getActive()) { continue; }
			constMap->dirLights[lightNum].dir2Light = -XMVector3Normalize(i.getDir());
			constMap->dirLights[lightNum].color = i.getColor();
			++lightNum;
		}
		constMap->activeDirLightCount = lightNum;

		lightNum = 0u;
		for (auto& i : pointLights)
		{
			if (!i.getActive()) { continue; }
			constMap->pointLights[lightNum].pos = i.getPos();
			constMap->pointLights[lightNum].color = i.getColor();
			constMap->pointLights[lightNum].atten = i.getAtten();
			++lightNum;
		}
		constMap->activePointLightCount = lightNum;

		lightNum = 0u;
		for (auto& i : spotLights)
		{
			if (!i.getActive()) { continue; }
			constMap->spotLights[lightNum].invLightDirNormal = -XMVector3Normalize(i.dir);
			constMap->spotLights[lightNum].pos = i.pos;
			constMap->spotLights[lightNum].color = i.color;
			constMap->spotLights[lightNum].atten = i.atten;
			constMap->spotLights[lightNum].factorAngleCos = i.factorAngleCos;
			++lightNum;
		}
		constMap->activeSpotLightCount = lightNum;

		constBuff->Unmap(0, nullptr);
	}
}

void Light::update()
{
	if (dirty)
	{
		transferConstBuffer();
		dirty = false;
	}
}

void Light::draw(UINT rootParamIndex)
{
	dxBase->getCmdList()->SetGraphicsRootConstantBufferView(rootParamIndex,
															constBuff->GetGPUVirtualAddress());
}