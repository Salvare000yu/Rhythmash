#include "PostEffect.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

// グローの半径
// 現在のピクセル -> サンプルする近隣のピクセルの距離
#define bloomR (10.f)

// これより大きい値の色がグローする
#define bloomThreshold (0.5f)

// 円周率
#define PI (3.141592653589793f)
#define PI2 (6.283185307179586f)

// 色を段階的にする
#define colorNum (256.f)

// 1 / 2.2
#define gamma (0.4545454545454545f)

#define slnSpeed (8.f)
#define slnDivLevel (96.f)
#define slnPower (24.f)

#define speedLineDivNum (1024.f)

// 1 ~
#define ditherColor (256.f)

//0 ~ 0.5
#define ditherNum (0.5f)

float3 getBloomPixel(SamplerState smp, float2 uv, float2 texPixelSize)
{
	float2 uv2 = floor(uv / texPixelSize) * texPixelSize;
	uv2 += texPixelSize * 0.001f;
	float3 tl = max(tex0.Sample(smp, uv2).rgb - bloomThreshold, 0.f);
	float3 tr = max(tex0.Sample(smp, uv2 + float2(texPixelSize.x, 0.f)).rgb - bloomThreshold, 0.f);
	float3 bl = max(tex0.Sample(smp, uv2 + float2(0.f, texPixelSize.y)).rgb - bloomThreshold, 0.f);
	float3 br = max(tex0.Sample(smp, uv2 + float2(texPixelSize.x, texPixelSize.y)).rgb - bloomThreshold, 0.f);
	float2 f = frac(uv / texPixelSize);

	float3 tA = lerp(tl, tr, f.x);
	float3 tB = lerp(bl, br, f.x);

	return lerp(tA, tB, f.y);
}

float3 getBloom(SamplerState smp, float2 uv, float2 texPixelSize)
{
	float3 bloomNum = float3(0.f, 0.f, 0.f);
	float2 off = float2(1.f, 1.f) * texPixelSize * bloomR;
	bloomNum += getBloomPixel(smp, uv + off * float2(-1.f, -1.f), texPixelSize * bloomR) * 0.3f;
	bloomNum += getBloomPixel(smp, uv + off * float2(-1.f, 0.f), texPixelSize * bloomR) * 0.5f;
	bloomNum += getBloomPixel(smp, uv + off * float2(-1.f, 1.f), texPixelSize * bloomR) * 0.3f;
	bloomNum += getBloomPixel(smp, uv + off * float2(0.f, -1.f), texPixelSize * bloomR) * 0.5f;
	bloomNum += getBloomPixel(smp, uv + off * float2(0.f, 0.f), texPixelSize * bloomR);
	bloomNum += getBloomPixel(smp, uv + off * float2(0.f, 1.f), texPixelSize * bloomR) * 0.5f;
	bloomNum += getBloomPixel(smp, uv + off * float2(1.f, -1.f), texPixelSize * bloomR) * 0.3f;
	bloomNum += getBloomPixel(smp, uv + off * float2(1.f, 0.f), texPixelSize * bloomR) * 0.5f;
	bloomNum += getBloomPixel(smp, uv + off * float2(1.f, 1.f), texPixelSize * bloomR) * 0.3f;
	bloomNum /= 4.f;
	return bloomNum;
}

float4 bloom(SamplerState smp, float2 uv, float intensity = 1.f)
{
	return float4(getBloom(smp, uv, 1.f / winSize) * intensity, 1.f);
}

float fracNoise(float2 coord)
{
	return frac(sin(dot(coord, float2(13.f, 80.f))) * 44000.f);
}

float vignatte(float2 uv)
{
	// UV値の中心からの距離(大きさ)
	float len = distance(uv, 0.5f);

	return -len * vignIntensity;
}

float noise(float2 uv, float time)
{
	float noiseNum = fracNoise(uv * time * 10.f) - 0.5f;
	return noiseNum * noizeIntensity;
}

float speedLine(float2 uv, float seed, float colourIntensity = 0.125f)
{
	// uv座標を-1 ~ +1に変換
	float2 pos = (uv - 0.5f) * 2.f;

	// 0 ~ 1の角度
	float angle = ((atan2(pos.r, pos.g) / PI) + 1.f) / 2.f;

	// 角度の値を段階的にする
	float floorAngle = floor(angle * speedLineDivNum) / speedLineDivNum * seed;

	// (段階的な)角度を参考にノイズを返す
	return saturate(fracNoise(float2(floorAngle, floorAngle))) * colourIntensity;
}
float4 dither(float4 texCol, float2 uv, float ditherSize = 1.f)
{
	float2 pixelSize = ditherSize / winSize;
		
	float2 num = floor(fmod(uv / pixelSize, 2.f));
	float c = fmod(num.x + num.y, 2.f);
		
	float4 retCol = texCol;
		
	retCol.r = (round(texCol.r * ditherColor + ditherNum) / ditherColor) * c;
	retCol.g = (round(texCol.g * ditherColor + ditherNum) / ditherColor) * c;
	retCol.b = (round(texCol.b * ditherColor + ditherNum) / ditherColor) * c;
	c = 1.f - c;
	retCol.r += (round(texCol.r * ditherColor - ditherNum) / ditherColor) * c;
	retCol.g += (round(texCol.g * ditherColor - ditherNum) / ditherColor) * c;
	retCol.b += (round(texCol.b * ditherColor - ditherNum) / ditherColor) * c;
		
	return retCol;
}

float3 chromaticSlice(float t)
{
	float3 ret = float3(1.f - t, 1.f - abs(t - 1.f), t - 1.f);
	return max(ret, 0.f);

}

float4 chromaticAberration(float2 uv, float level = 3.f, float spread = 0.03125f)
{
	float2 offset = (uv - 0.5f) * float2(1.f, -1.f);
	float3 sum = float3(0.f, 0.f, 0.f);
	float4 ret = float4(0.f, 0.f, 0.f, 1.f);
		
	for (float i = 0; i < level; i++)
	{
		float t = 2.f * i / float(level - 1); // range 0.0->2.0
		float3 slice = float3(1.f - t, 1.f - abs(t - 1.f), t - 1.f);
		slice = max(slice, 0.f);
		sum += slice;
		float2 slice_offset = (t - 1.f) * spread * offset;
			
		ret.rgb += slice * tex0.Sample(smp, uv + slice_offset).rgb;
	}
	ret.rgb /= sum;
	return float4(ret.rgb, 1.f);
}

float4 main(VSOutput input) : SV_TARGET
{
	// --------------------
	// モザイク
	// --------------------
	
	// 画面を左右それぞれmosaicNum分割した大きさのモザイクになる
	float2 uv = floor(input.uv * mosaicNum) / mosaicNum;
	// 時間[s]
	float time = nowTime / oneSec;

	// --------------------
	// ビネッタ
	// --------------------
	float vignNum = vignatte(uv);

	// --------------------
	// rgbずらし&ディザリング
	// --------------------
	const float ditherSize = winSize.y / 512.f;
	float4 texColor0 = dither(tex0.Sample(smp, uv), uv, ditherSize);
	texColor0.g = dither(tex0.Sample(smp, uv + rgbShiftNum), uv, ditherSize).g;
	texColor0.rgb = pow(texColor0.rgb, gamma);

	float noiseNum = noise(input.uv, time);
	
	// --------------------
	// 集中線
	// --------------------
	
	// 時間で動いてほしいのでseedには時間を入れる
	float speedLineNum = speedLine(input.uv, fmod(nowTime / oneSec, 1.f), speedLineIntensity);
	// 中心に近いほど色を薄くする
	speedLineNum *= distance(float2(0.5f, 0.5f), input.uv);
	
	// --------------------
	// 合わせる
	// --------------------
	
	float4 drawCol = float4(texColor0.rgb + vignNum + noiseNum + speedLineNum, alpha);
	
	// 色数を減らす
	drawCol.rgb = floor(drawCol.rgb * colorNum) / colorNum;
	
	// ブルーム
	drawCol.rgb += bloom(smp, uv).rgb;

	return drawCol;
}
