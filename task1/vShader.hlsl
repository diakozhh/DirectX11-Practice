#include "sceneBuffer.hlsli"
#include "geomBuffer.hlsli"

struct VSInput
{
  float3 position : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  uint instanceId : SV_InstanceID;
};

struct VSOutput
{
  float4 position : SV_Position;
  float4 worldPos : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  nointerpolation uint instanceId : INST_ID;
};

VSOutput main(VSInput input)
{
  VSOutput output;
  unsigned int idx = indexBuffer[input.instanceId].x;
  output.worldPos = mul(geomBuffer[idx].world, float4(input.position, 1.0f));

  output.position = mul(viewProj, output.worldPos);
  output.texCoord = input.texCoord;

  output.normal = mul(geomBuffer[idx].norm, float4(input.normal, 0.0f)).xyz;
  output.tangent = mul(geomBuffer[idx].norm, float4(input.tangent, 0.0f)).xyz;
  output.instanceId = idx;


  return output;
}