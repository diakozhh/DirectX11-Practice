#include "defines.hlsli"

cbuffer SceneConstantBuffer : register (b1)
{
  float4x4 viewProj;
  float4 planes[6];
};

cbuffer IndexBuffer : register(b2)
{
  uint4 indexBuffer[MAX_CUBES]; // x - index
}
