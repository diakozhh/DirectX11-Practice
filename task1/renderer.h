#pragma once

#include <d3d11.h>
#include <dxgi.h>
class Renderer {
private:
  ID3D11Device* m_pDevice = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  IDXGISwapChain* m_pSwapChain = nullptr;
  ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
  UINT m_width = 0;
  UINT m_height = 0;

  HRESULT setupBackBuffer();

public:
  bool deviceInit(HWND hWnd);
  bool render();
  void deviceCleanup();
  bool winResize(UINT width, UINT height);
};