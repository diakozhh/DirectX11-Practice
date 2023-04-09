#pragma once
#include <directxmath.h>
using namespace DirectX;

class Frustum {
public:
  // Function to initialize frustum class
  void Init(float screenDepth) { m_screenDepth = screenDepth; };
  // Release function
  void Release() {};
  // Function to build frustum
  void ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

  // Functions to check if rectengle is in frustum
  bool CheckRectangle(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);
private:
  float m_screenDepth;
  float m_planes[6][4];
};