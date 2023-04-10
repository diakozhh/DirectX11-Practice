#include "frustum.h"

// Function to build frustum

void Frustum::ConstructFrustum(XMMATRIX viewMatrix, XMMATRIX projectionMatrix) {
  // Convert the projection matrix into a 4x4 float type.
  XMFLOAT4X4 pMatrix;
  XMStoreFloat4x4(&pMatrix, projectionMatrix);

  // Calculate the minimum Z distance in the frustum.
  float zMinimum = -pMatrix._43 / pMatrix._33;
  float r = m_screenDepth / (m_screenDepth - zMinimum);

  // Load the updated values back into the projection matrix.
  pMatrix._33 = r;
  pMatrix._43 = -r * zMinimum;
  projectionMatrix = XMLoadFloat4x4(&pMatrix);

  // Create the frustum matrix from the view matrix and updated projection matrix.
  XMMATRIX finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

  // Convert the final matrix into a 4x4 float type.
  XMFLOAT4X4 matrix;
  XMStoreFloat4x4(&matrix, finalMatrix);

  // Calculate near plane of frustum.
  m_planes[0].x = matrix._14 + matrix._13;
  m_planes[0].y = matrix._24 + matrix._23;
  m_planes[0].z = matrix._34 + matrix._33;
  m_planes[0].w = matrix._44 + matrix._43;

  // Normalize the near plane.
  float length = sqrtf((m_planes[0].x * m_planes[0].x) + (m_planes[0].y * m_planes[0].y) + (m_planes[0].z * m_planes[0].z));
  m_planes[0].x /= length;
  m_planes[0].y /= length;
  m_planes[0].z /= length;
  m_planes[0].w /= length;

  // Calculate far plane of frustum.
  m_planes[1].x = matrix._14 - matrix._13;
  m_planes[1].y = matrix._24 - matrix._23;
  m_planes[1].z = matrix._34 - matrix._33;
  m_planes[1].w = matrix._44 - matrix._43;

  // Normalize the far plane.
  length = sqrtf((m_planes[1].x * m_planes[1].x) + (m_planes[1].y * m_planes[1].y) + (m_planes[1].z * m_planes[1].z));
  m_planes[1].x /= length;
  m_planes[1].y /= length;
  m_planes[1].z /= length;
  m_planes[1].w /= length;

  // Calculate left plane of frustum.
  m_planes[2].x = matrix._14 + matrix._11;
  m_planes[2].y = matrix._24 + matrix._21;
  m_planes[2].z = matrix._34 + matrix._31;
  m_planes[2].w = matrix._44 + matrix._41;

  // Normalize the left plane.
  length = sqrtf((m_planes[2].x * m_planes[2].x) + (m_planes[2].y * m_planes[2].y) + (m_planes[2].z * m_planes[2].z));
  m_planes[2].x /= length;
  m_planes[2].y /= length;
  m_planes[2].z /= length;
  m_planes[2].w /= length;

  // Calculate right plane of frustum.
  m_planes[3].x = matrix._14 - matrix._11;
  m_planes[3].y = matrix._24 - matrix._21;
  m_planes[3].z = matrix._34 - matrix._31;
  m_planes[3].w = matrix._44 - matrix._41;

  // Normalize the right plane.
  length = sqrtf((m_planes[3].x * m_planes[3].x) + (m_planes[3].y * m_planes[3].y) + (m_planes[3].z * m_planes[3].z));
  m_planes[3].x /= length;
  m_planes[3].y /= length;
  m_planes[3].z /= length;
  m_planes[3].w /= length;

  // Calculate top plane of frustum.
  m_planes[4].x = matrix._14 - matrix._12;
  m_planes[4].y = matrix._24 - matrix._22;
  m_planes[4].z = matrix._34 - matrix._32;
  m_planes[4].w = matrix._44 - matrix._42;

  // Normalize the top plane.
  length = sqrtf((m_planes[4].x * m_planes[4].x) + (m_planes[4].y * m_planes[4].y) + (m_planes[4].z * m_planes[4].z));
  m_planes[4].x /= length;
  m_planes[4].y /= length;
  m_planes[4].z /= length;
  m_planes[4].w /= length;

  // Calculate bottom plane of frustum.
  m_planes[5].x = matrix._14 + matrix._12;
  m_planes[5].y = matrix._24 + matrix._22;
  m_planes[5].z = matrix._34 + matrix._32;
  m_planes[5].w = matrix._44 + matrix._42;

  // Normalize the bottom plane.
  length = sqrtf((m_planes[5].x * m_planes[5].x) + (m_planes[5].y * m_planes[5].y) + (m_planes[5].z * m_planes[5].z));
  m_planes[5].x /= length;
  m_planes[5].y /= length;
  m_planes[5].z /= length;
  m_planes[5].w /= length;
}
