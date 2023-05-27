#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

// ディザリング抜き
void ScreenDoor(float2 screenPos, float alpha)
{
	static const int Bayer[4][4] =
	{
		{ 0, 8, 2, 10 },
		{ 12, 4, 14, 6 },
		{ 3, 11, 1, 9 },
		{ 15, 7, 13, 5 }
	};
	
	// 0 ~ 16
	float ditherLevel = clamp(16.f - (alpha * 16.f), 0.f, 16.f);
		
	int ditherUvX = (int) fmod(screenPos.x, 4.f);
	int ditherUvY = (int) fmod(screenPos.y, 4.f);
	float doorNum = Bayer[ditherUvY][ditherUvX];
	clip(doorNum - ditherLevel);
}

PSOutput main(VSOutput input)
{
	float4 texcolor = float4(tex.Sample(smp, input.uv * texTilling + shiftUv));

	ScreenDoor(input.svpos.xy, texcolor.a * color.a);
	
	const float shininess = 4.f; // 光沢
	float3 eyeDir = normalize(cameraPos - input.worldPos.xyz); // 頂点->視点ベクトル
	
	float3 ambient = m_ambient;
	float4 shadeColor = float4(ambientColor * ambient, m_alpha);
	
	for (uint d = 0; d < activeDirLightCount; ++d)
	{
		// ライトへ向かうベクトルと法線の内積
		float3 dir2LightDotNormal = dot(dirLights[d].dir2Light, input.normal);
		float3 reflect = normalize(-dirLights[d].dir2Light + 2 * dir2LightDotNormal * input.normal);
		float3 diffuse = dir2LightDotNormal * m_diffuse;
		float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular;
		
		shadeColor.rgb += (diffuse + specular) * dirLights[d].color;
	}
	
	for (uint p = 0; p < activePointLightCount; ++p)
	{
		float3 dir2Light = pointLights[p].pos - input.worldPos.xyz;
		float l = length(dir2Light);
		dir2Light = normalize(dir2Light);
		
		float atten = 1.f / (pointLights[p].atten.x + pointLights[p].atten.y * l + pointLights[p].atten.z * l * l);
		
		// ライトへ向かうベクトルと法線の内積
		float3 dir2LightDotNormal = dot(dir2Light, input.normal);

		float3 reflect = normalize(-dir2Light + 2 * dir2LightDotNormal * input.normal); // 反射光
		float3 diffuse = dir2LightDotNormal * m_diffuse;
		float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular; // 鏡面反射光

		shadeColor.rgb += atten * (diffuse + specular) * pointLights[p].color.rgb;
	}
	
	for (uint s = 0; s < activeSpotLightCount; ++s)
	{
		// ライトへの方向ベクトル
		float3 dir2Light = spotLights[s].pos - input.worldPos.xyz;
		float d = length(dir2Light);
		dir2Light = normalize(dir2Light);

		// 距離減衰係数
		float atten = saturate(1.f / (spotLights[s].atten.x + spotLights[s].atten.y * d + spotLights[s].atten.z * d * d));

		// 角度減衰
		float cos = dot(dir2Light, spotLights[s].invLightDirNormal);
		// 減衰開始角度から、減衰終了角度にかけて減衰
		// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
		float angleatten = smoothstep(spotLights[s].factorAngleCos.y, spotLights[s].factorAngleCos.x, cos);
		// 角度減衰を乗算
		atten *= angleatten;

		// ライトに向かうベクトルと法線の内積
		float3 dir2LightDotNormal = dot(dir2Light, input.normal);
		// 反射光ベクトル
		float3 reflect = normalize(-dir2Light + 2 * dir2LightDotNormal * input.normal); // 反射光
		// 拡散反射光
		float3 diffuse = dir2LightDotNormal * m_diffuse;
		// 鏡面反射光
		float3 specular = pow(saturate(dot(reflect, eyeDir)), shininess) * m_specular; // 鏡面反射光

		// 全て加算する
		shadeColor.rgb += atten * (diffuse + specular) * spotLights[s].color;
	}
	
	for (uint c = 0; c < activeCircleShadowCount; ++c)
	{
		// オブジェクト表面からキャスターへのベクトル
		float3 casterv = circleShadows[c].casterPos - input.worldPos.xyz;
		// 光線方向での距離
		float d = dot(casterv, circleShadows[c].invLightDirNormal);

		// 距離減衰係数
		float atten = saturate(1.f / (circleShadows[c].atten.x + circleShadows[c].atten.y * d + circleShadows[c].atten.z * d * d));
		// 距離がマイナスなら0にする
		atten *= step(0, d);

		// ライトの座標
		float3 lightpos = circleShadows[c].casterPos + circleShadows[c].invLightDirNormal * circleShadows[c].caster2LightDistance;
		//  オブジェクト表面からライトへのベクトル（単位ベクトル）
		float3 lightv = normalize(lightpos - input.worldPos.xyz);
		// 角度減衰
		float cos = dot(lightv, circleShadows[c].invLightDirNormal);
		// 減衰開始角度から、減衰終了角度にかけて減衰
		// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
		float angleatten = smoothstep(circleShadows[c].factorAngleCos.y, circleShadows[c].factorAngleCos.x, cos);
		// 角度減衰を乗算
		atten *= angleatten;

		// 全て減算する
		shadeColor.rgb -= atten;
	}
	
	PSOutput output;
	output.target0 = shadeColor * texcolor * color;
	output.target1 = float4((input.normal + 1.f) / 2.f, 1);

	return output;
}
