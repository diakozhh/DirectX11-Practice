#include <assert.h>

#include "renderer.h"

#define SAFE_RELEASE(p) \
if (p != NULL) { \
    p->Release(); \
    p = NULL;\
}

HRESULT Renderer::setupBackBuffer() {
  ID3D11Texture2D* pBackBuffer = NULL;
  HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
  assert(SUCCEEDED(hr));
  if (SUCCEEDED(hr)) {
    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackBufferRTV);
    assert(SUCCEEDED(hr));

    SAFE_RELEASE(pBackBuffer);
  }
  return hr;
}

bool Renderer::deviceInit(HWND hWnd) {

  HRESULT hr;

  IDXGIFactory* pFactory = nullptr;
  hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);

  //select adapter
  IDXGIAdapter* pSelectedAdapter = NULL;
  if (SUCCEEDED(hr)) {
    IDXGIAdapter* pAdapter = NULL;
    UINT adapterIndex = 0;
    while (SUCCEEDED(pFactory->EnumAdapters(adapterIndex, &pAdapter))) {
      DXGI_ADAPTER_DESC desc;
      pAdapter->GetDesc(&desc);
      if (wcscmp(desc.Description, L"Microsoft Basic Render Driver") != 0) {
        pSelectedAdapter = pAdapter;
        break;
      }
      pAdapter->Release();
      adapterIndex++;
    }
  }
  assert(pSelectedAdapter != NULL);

  //create DirectX 11 device
  D3D_FEATURE_LEVEL level;
  D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
  if (SUCCEEDED(hr)) {
    UINT flags = 0;
#ifdef _DEBUG
    //  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(pSelectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
      flags, levels, 1, D3D11_SDK_VERSION, &m_pDevice, &level, &m_pDeviceContext);
    assert(level == D3D_FEATURE_LEVEL_11_0);
    assert(SUCCEEDED(hr));
  }
  //create swapchain
  if (SUCCEEDED(hr)) {
    RECT rc;
    GetClientRect(hWnd, &rc);
    m_width = rc.right - rc.left;
    m_height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = m_width;
    swapChainDesc.BufferDesc.Height = m_height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    hr = pFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
    assert(SUCCEEDED(hr));
  }
  if (SUCCEEDED(hr)) {
    hr = setupBackBuffer();
  }

  SAFE_RELEASE(pSelectedAdapter);
  SAFE_RELEASE(pFactory);

  return SUCCEEDED(hr);
}


bool Renderer::render() {
  m_pDeviceContext->ClearState();

  ID3D11RenderTargetView* views[] = { m_pBackBufferRTV };
  m_pDeviceContext->OMSetRenderTargets(1, views, nullptr);
  static const FLOAT BackColor[4] = { 0.75f, 0.25f, 0.25f, 1.0f };
  m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, BackColor);

  HRESULT hr = m_pSwapChain->Present(1, 0);
  assert(SUCCEEDED(hr));

  return SUCCEEDED(hr);
}

void Renderer::deviceCleanup() {
  SAFE_RELEASE(m_pBackBufferRTV);
  SAFE_RELEASE(m_pSwapChain);
  SAFE_RELEASE(m_pDeviceContext);
  SAFE_RELEASE(m_pDevice);
}

bool Renderer::winResize(UINT width, UINT height) {
  if (width != m_width || height != m_height) {
    SAFE_RELEASE(m_pBackBufferRTV);

    HRESULT hr = m_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    assert(SUCCEEDED(hr));
    if (SUCCEEDED(hr)) {
      m_width = width;
      m_height = height;

      hr = setupBackBuffer();
    }
    return SUCCEEDED(hr);
  }
  return true;
}
