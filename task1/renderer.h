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
#include "renderTexture.h"
#include "postEffect.h"
#include "frustum.h"

constexpr const std::size_t maxCubeNumber = 100;

struct Vertex {
  DirectX::XMFLOAT3 pos;
  DirectX::XMFLOAT2 uv;
  DirectX::XMFLOAT3 normal;
  DirectX::XMFLOAT3 tangent;
};

static const XMFLOAT4 AABB[] = {
  {-0.5f, -0.5f, -0.5f, 1.0f},
  {0.5f, -0.5f, -0.5f, 1.0f},
  {-0.5f, 0.5f, -0.5f, 1.0f},
  {-0.5f, -0.5f, 0.5f, 1.0f},
  {0.5f, 0.5f, -0.5f, 1.0f},
  {0.5f, -0.5f, 0.5f, 1.0f},
  {-0.5f, 0.5f, 0.5f, 1.0f},
  {0.5f,  0.5f, 0.5f, 1.0f}
};

struct GeomMatrixBuffer {
  DirectX::XMMATRIX mWorldMatrix;
  DirectX::XMMATRIX norm;
  DirectX::XMFLOAT4 shineSpeedTexIdNm;
};

struct SceneMatrixBuffer {
  XMMATRIX mViewProjectionMatrix;
  DirectX::XMINT4 indexBuffer[maxCubeNumber];
};

struct LightMatrixBuffer {
  DirectX::XMFLOAT4 cameraPosition;
  DirectX::XMINT4 lightCount;
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
  ID3D11Buffer* m_pGeomMatrixBuffer = nullptr;
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

  std::shared_ptr<RenderTexture> m_pRenderTexture;
  std::shared_ptr<PostEffect> m_pPostEffect;

  struct CubeModel
  {
    DirectX::XMFLOAT4 pos;
    DirectX::XMFLOAT4 shineSpeedIdNm;
  };
  std::vector<CubeModel> m_pCubeModelVector;
  std::vector<int> m_cubeIndexies;
  
  Frustum* m_pFrustum = nullptr;

  HRESULT setupBackBuffer();
  HRESULT initScene(HWND hWnd);
  void inputMovement();

public:
  bool deviceInit(HINSTANCE hinst, HWND hWnd, Camera* pCamera, Input* pInput);
  bool getState();
  bool render();
  void deviceCleanup();
  bool winResize(UINT width, UINT height);
};