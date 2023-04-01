#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <directxmath.h>
#include <memory>

#include "camera.h"
#include "input.h"
#include "utils.h"
#include "cubeMap.h"
#include "texture.h"
#include "lights.h"

struct Vertex {
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT2 uv;
  DirectX::XMFLOAT3 normal;
  DirectX::XMFLOAT3 tangent;
};

struct WorldMatrixBuffer {
  XMMATRIX mWorldMatrix;
  DirectX::XMFLOAT4 shine;
};

struct SceneMatrixBuffer {
  XMMATRIX mViewProjectionMatrix;
  DirectX::XMFLOAT4 cameraPosition;
};

struct LightMatrixBuffer {
  int lightCount[4];
  DirectX::XMFLOAT4 lightPositions[maxLightNumber];
  DirectX::XMFLOAT4 lightColors[maxLightNumber];
  DirectX::XMFLOAT4 ambientColor;
};

struct TransparentWorldBuffer
{
  DirectX::XMMATRIX worldMatrix;
  DirectX::XMFLOAT4 color;
};

struct VertexPos
{
  float x, y, z;
};

class Renderer {
private:
  ID3D11Device* m_pDevice = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  IDXGISwapChain* m_pSwapChain = nullptr;
  ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;

  ID3D11Buffer* m_pIndexBuffer = nullptr;
  ID3D11Buffer* m_pVertexBuffer = nullptr;

  ID3D11VertexShader* m_pVertexShader = nullptr;
  ID3D11PixelShader* m_pPixelShader = nullptr;

  ID3D11InputLayout* m_pInputLayout = nullptr;

  ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
  ID3D11Buffer* m_pWorldMatrixBuffer1 = nullptr;

  ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
  ID3D11Buffer* m_pLightMatrixBuffer = nullptr;
  ID3D11RasterizerState* m_pRasterizerState = nullptr;
  ID3D11SamplerState* m_pSampler = nullptr;
  
  ID3D11DepthStencilState* m_pDepthState = nullptr;

  ID3D11VertexShader* m_pTransparentVertexShader = nullptr;
  ID3D11PixelShader* m_pTransparentPixelShader = nullptr;
  ID3D11InputLayout* m_pTransparentInputLayout = nullptr;
  ID3D11Buffer* m_pTransparentVertexBuffer = nullptr;
  ID3D11Buffer* m_pTransparentIndexBuffer = nullptr;
  ID3D11Buffer* m_pTransparentWorldBuffer = nullptr;
  ID3D11Buffer* m_pTransparentWorldBuffer1 = nullptr;
  ID3D11Buffer* m_pTransparentSceneBuffer = nullptr;
  ID3D11RasterizerState* m_pTransparentRasterizerState = nullptr;
  ID3D11DepthStencilState* m_pTransparentDepthState = nullptr;
  ID3D11BlendState* m_pTransparentBlendState = nullptr;

  ID3D11Texture2D* m_pDepthBuffer = nullptr;
  ID3D11DepthStencilView* m_pDepthBufferDSV = nullptr;

  static constexpr const DirectX::XMFLOAT4 ambientColor_{ 0.27f, 0.05f, 0.81f, 1.0f };

  UINT m_width = 1280;
  UINT m_height = 720;

  Camera* m_pCamera = nullptr;
  Input* m_pInput = nullptr;

  CubeMap* m_pCubeMap = nullptr;
  std::vector<Texture> m_textureArray;
  std::shared_ptr<Lights> m_pLights;

  HRESULT setupBackBuffer();
  HRESULT initScene();
  void inputMovement();

public:
  bool deviceInit(HINSTANCE hinst, HWND hWnd, Camera* pCamera, Input* pInput);
  bool getState();
  bool render();
  void deviceCleanup();
  bool winResize(UINT width, UINT height);
};