#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <d3d11.h>
#include <directxmath.h>
#include <string>

#include "camera.h"
#include "input.h"

struct Vertex {
  float x, y, z;
  COLORREF color;
};

struct WorldMatrixBuffer {
  XMMATRIX mWorldMatrix;
};

struct SceneMatrixBuffer {
  XMMATRIX mViewProjectionMatrix;
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
  ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
  ID3D11RasterizerState* m_pRasterizerState = nullptr;

  Camera* m_pCamera = nullptr;
  Input* m_pInput = nullptr;

  UINT m_width = 0;
  UINT m_height = 0;

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