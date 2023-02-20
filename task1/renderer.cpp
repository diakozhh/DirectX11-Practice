#include <assert.h>
#include <d3dcompiler.h>

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

HRESULT Renderer::initScene() {
  HRESULT hr = S_OK;
  
  static const Vertex Vertices[] = {
        {-0.5f, -0.5f, 0.0f, RGB(255, 0, 0)},
        { 0.5f, -0.5f, 0.0f, RGB(0, 255, 0)},
        { 0.0f,  0.5f, 0.0f, RGB(0, 0, 255)}
  };
  
  static const USHORT Indices[] = {
      0, 2, 1
  };

  static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0,  DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} };


  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(Vertices);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &Vertices;
    data.SysMemPitch = sizeof(Vertices);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pVertexBuffer);
    assert(SUCCEEDED(hr));
  }

  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(Indices);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &Indices;
    data.SysMemPitch = sizeof(Indices);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pIndexBuffer);
    assert(SUCCEEDED(hr));
  }

  ID3D10Blob* vShaderBuffer = nullptr;
  ID3D10Blob* pShaderBuffer = nullptr;
  
  int flags = 0;
  #ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
  #endif
  
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"vShader.hlsl", NULL, NULL, "main", "vs_5_0", flags, 0, &vShaderBuffer, NULL);
    hr = m_pDevice->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
  }
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"pShader.hlsl", NULL, NULL, "main", "ps_5_0", flags, 0, &pShaderBuffer, NULL);
    hr = m_pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
  }
  if (SUCCEEDED(hr)) {
    int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
    hr = m_pDevice->CreateInputLayout(InputDesc, numElements, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), &m_pInputLayout);
  }

  SAFE_RELEASE(vShaderBuffer);
  SAFE_RELEASE(pShaderBuffer);

  return hr;

  return E_NOTIMPL;
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
      flags |= D3D11_CREATE_DEVICE_DEBUG;
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
  
  if (SUCCEEDED(hr)) {
    hr = initScene();
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

  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.Width = (FLOAT)m_width;
  vp.Height = (FLOAT)m_height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;

  m_pDeviceContext->RSSetViewports(1, &vp);

  D3D11_RECT rect;
  rect.left = 0;
  rect.right = m_width;
  rect.top = 0;
  rect.bottom = m_height;

  m_pDeviceContext->RSSetScissorRects(1, &rect);

  m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

  ID3D11Buffer* vBuffer[] = {m_pVertexBuffer};
  UINT strides[] = {16};
  UINT offsets[] = {0};

  m_pDeviceContext->IASetVertexBuffers(0, 1, vBuffer, strides, offsets);
  m_pDeviceContext->IASetInputLayout(m_pInputLayout);
  m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
  m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
  m_pDeviceContext->DrawIndexed(3, 0, 0);

  HRESULT hr = m_pSwapChain->Present(1, 0);
  assert(SUCCEEDED(hr));

  return SUCCEEDED(hr);
}

void Renderer::deviceCleanup() {
  SAFE_RELEASE(m_pBackBufferRTV);
  SAFE_RELEASE(m_pSwapChain);
  SAFE_RELEASE(m_pDeviceContext);

  SAFE_RELEASE(m_pIndexBuffer);
  SAFE_RELEASE(m_pVertexBuffer);
  SAFE_RELEASE(m_pVertexShader);
  SAFE_RELEASE(m_pPixelShader);

  SAFE_RELEASE(m_pInputLayout);
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
