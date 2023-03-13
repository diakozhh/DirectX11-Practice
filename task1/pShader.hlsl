Texture2D cubeTexture : register (t0);

SamplerState cubeSampler : register(s0);

struct VSOutput {
  float4 position : SV_POSITION;
  float2 uv : TEXCOORD;
};

float4 main(VSOutput input) : SV_TARGET {
  return float4(cubeTexture.Sample(cubeSampler, input.uv).xyz, 1.0);
}