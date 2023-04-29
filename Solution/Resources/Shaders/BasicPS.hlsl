#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

PSOutput main(VSOutput input)
{
	float4 texcolor = float4(tex.Sample(smp, input.uv * texTilling + shiftUv));
	
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
	
	PSOutput output;
	output.target0 = shadeColor * texcolor * color;
	// target1を反転色にする
	output.target1 = output.target0;

	return output;
}
