#pragma once

#include <directxmath.h>
#include <stdint.h>
#include <functional>
#include <vector>
static const constexpr std::size_t maxLightNumber = 10;

struct LightInfo
{
  DirectX::XMFLOAT4 position;
  DirectX::XMFLOAT4 color;
};

class Lights
{
public:
  bool Add(const LightInfo& info);
  std::size_t GetNumber();
  std::vector<DirectX::XMFLOAT4> GetPositions();
  std::vector<DirectX::XMFLOAT4> GetColors();

private:
  std::vector<LightInfo> lights;
};