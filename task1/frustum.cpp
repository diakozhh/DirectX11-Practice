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
  m_planes[0][0] = matrix._14 + matrix._13;
  m_planes[0][1] = matrix._24 + matrix._23;
  m_planes[0][2] = matrix._34 + matrix._33;
  m_planes[0][3] = matrix._44 + matrix._43;

  // Normalize the near plane.
  float length = sqrtf((m_planes[0][0] * m_planes[0][0]) + (m_planes[0][1] * m_planes[0][1]) + (m_planes[0][2] * m_planes[0][2]));
  m_planes[0][0] /= length;
  m_planes[0][1] /= length;
  m_planes[0][2] /= length;
  m_planes[0][3] /= length;

  // Calculate far plane of frustum.
  m_planes[1][0] = matrix._14 - matrix._13;
  m_planes[1][1] = matrix._24 - matrix._23;
  m_planes[1][2] = matrix._34 - matrix._33;
  m_planes[1][3] = matrix._44 - matrix._43;

  // Normalize the far plane.
  length = sqrtf((m_planes[1][0] * m_planes[1][0]) + (m_planes[1][1] * m_planes[1][1]) + (m_planes[1][2] * m_planes[1][2]));
  m_planes[1][0] /= length;
  m_planes[1][1] /= length;
  m_planes[1][2] /= length;
  m_planes[1][3] /= length;

  // Calculate left plane of frustum.
  m_planes[2][0] = matrix._14 + matrix._11;
  m_planes[2][1] = matrix._24 + matrix._21;
  m_planes[2][2] = matrix._34 + matrix._31;
  m_planes[2][3] = matrix._44 + matrix._41;

  // Normalize the left plane.
  length = sqrtf((m_planes[2][0] * m_planes[2][0]) + (m_planes[2][1] * m_planes[2][1]) + (m_planes[2][2] * m_planes[2][2]));
  m_planes[2][0] /= length;
  m_planes[2][1] /= length;
  m_planes[2][2] /= length;
  m_planes[2][3] /= length;

  // Calculate right plane of frustum.
  m_planes[3][0] = matrix._14 - matrix._11;
  m_planes[3][1] = matrix._24 - matrix._21;
  m_planes[3][2] = matrix._34 - matrix._31;
  m_planes[3][3] = matrix._44 - matrix._41;

  // Normalize the right plane.
  length = sqrtf((m_planes[3][0] * m_planes[3][0]) + (m_planes[3][1] * m_planes[3][1]) + (m_planes[3][2] * m_planes[3][2]));
  m_planes[3][0] /= length;
  m_planes[3][1] /= length;
  m_planes[3][2] /= length;
  m_planes[3][3] /= length;

  // Calculate top plane of frustum.
  m_planes[4][0] = matrix._14 - matrix._12;
  m_planes[4][1] = matrix._24 - matrix._22;
  m_planes[4][2] = matrix._34 - matrix._32;
  m_planes[4][3] = matrix._44 - matrix._42;

  // Normalize the top plane.
  length = sqrtf((m_planes[4][0] * m_planes[4][0]) + (m_planes[4][1] * m_planes[4][1]) + (m_planes[4][2] * m_planes[4][2]));
  m_planes[4][0] /= length;
  m_planes[4][1] /= length;
  m_planes[4][2] /= length;
  m_planes[4][3] /= length;

  // Calculate bottom plane of frustum.
  m_planes[5][0] = matrix._14 + matrix._12;
  m_planes[5][1] = matrix._24 + matrix._22;
  m_planes[5][2] = matrix._34 + matrix._32;
  m_planes[5][3] = matrix._44 + matrix._42;

  // Normalize the bottom plane.
  length = sqrtf((m_planes[5][0] * m_planes[5][0]) + (m_planes[5][1] * m_planes[5][1]) + (m_planes[5][2] * m_planes[5][2]));
  m_planes[5][0] /= length;
  m_planes[5][1] /= length;
  m_planes[5][2] /= length;
  m_planes[5][3] /= length;
}

bool Frustum::CheckRectangle(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth) {
  // Check if any of the 6 planes of the rectangle are inside the view frustum.
  for (int i = 0; i < 6; i++) {
    float dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
    if (dotProduct >= 0.0f) {
      continue;
    }

    return false;
  }

  return true;
}