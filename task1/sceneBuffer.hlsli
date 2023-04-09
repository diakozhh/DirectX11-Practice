#include "defines.hlsli"

cbuffer SceneConstantBuffer : register (b1)
{
  float4x4 viewProj;
  int4 indexBuffer[MAX_CUBES]; // x - index
};
/*/cbuffer SceneBuffer : register (b1)
{
  float4x4 viewProj;
  float4 cameraPos;
};
cbuffer LightBuffer : register (b2)
{
  int4 lightCount;  // x - count, y - use normals, z - show normals
  float4 lightPos[10];
  float4 lightColor[10];
  float4 ambientColor;
};*/