#include "geomBuffer.hlsli"
#include "sceneBuffer.hlsli"

cbuffer CullParams : register(b0)
{
  uint4 numShapes; // x - objects count;
  float4 bbMin[MAX_CUBES];
  float4 bbMax[MAX_CUBES];
}

RWStructuredBuffer<uint> indirectArgs : register(u0);
RWStructuredBuffer<uint4> objectsIds : register(u1);

bool IsBoxInside(in float4 planes[6], in float3 bbMin, in float3 bbMax) {
  for (int i = 0; i < 6; i++) {
    float3 norm = planes[i].xyz;
    float4 p = float4(
      norm.x < 0 ? bbMin.x : bbMax.x,
      norm.y < 0 ? bbMin.y : bbMax.y,
      norm.z < 0 ? bbMin.z : bbMax.z,
      1.0f
      );
    float s = dot(p, planes[i]);
    if (s < 0.0f) {
      return false;
    }
  }
  return true;
}

[numthreads(64, 1, 1)]
void main(uint3 globalThreadId : SV_DispatchThreadID)
{
  if (globalThreadId.x >= numShapes.x) {
    return;
  }
  if (IsBoxInside(planes, bbMin[globalThreadId.x].xyz, bbMax[globalThreadId.x].xyz)) {
    uint id = 0;
    InterlockedAdd(indirectArgs[1], 1, id);
    objectsIds[id] = uint4(globalThreadId.x, 0, 0, 0);
  }
}