#pragma once

#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>

#include "utils.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

class CubeMap {
private:
  struct Vertex {
    float x, y, z;
  };
  struct WorldMatrixBuffer {
    XMMATRIX mWorldMatrix;
    XMFLOAT4 size;
  };
  struct SceneMatrixBuffer {
    XMMATRIX mViewProjectionMatrix;
    XMFLOAT4 cameraPos;
  };

  ID3D11Buffer* m_pVertexBuffer;
  ID3D11Buffer* m_pIndexBuffer;
  ID3D11Buffer* m_pWorldMatrixBuffer;
  ID3D11Buffer* m_pSceneMatrixBuffer;
  ID3D11RasterizerState* m_pRasterizerState;
  ID3D11SamplerState* m_pSampler;

  ID3D11InputLayout* m_pInputLayout;
  ID3D11VertexShader* m_pVertexShader;
  ID3D11PixelShader* m_pPixelShader;

  ID3D11ShaderResourceView* m_pTexture;

  int m_numSphereVertices;
  int m_numSphereFaces;
  float m_radius;
public:
  CubeMap(ID3D11Device* device, ID3D11DeviceContext* context, int screenWidth, int screenHeight, UINT LatLines, UINT LongLines);
  ~CubeMap();

  void resize(int screenWidth, int screenHeight);
  void render(ID3D11DeviceContext* context);
  bool getState(ID3D11DeviceContext* context, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMFLOAT3 cameraPos);
};