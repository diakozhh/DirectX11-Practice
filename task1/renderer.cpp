#include <assert.h>
#include <d3dcompiler.h>
#include <array>
#include <chrono>
#include <math.h>

#include "renderer.h"
#include "D3DInclude.h"
namespace {
  static const Vertex Vertices[] = {
        {{-0.5, -0.5,  0.5}, {0,1}, {0,-1,0}, {1,0,0}},
        {{ 0.5, -0.5,  0.5}, {1,1}, {0,-1,0}, {1,0,0}},
        {{ 0.5, -0.5, -0.5}, {1,0}, {0,-1,0}, {1,0,0}},
        {{-0.5, -0.5, -0.5}, {0,0}, {0,-1,0}, {1,0,0}},

        {{-0.5,  0.5, -0.5}, {0,1}, {0,1,0}, {1,0,0}},
        {{ 0.5,  0.5, -0.5}, {1,1}, {0,1,0}, {1,0,0}},
        {{ 0.5,  0.5,  0.5}, {1,0}, {0,1,0}, {1,0,0}},
        {{-0.5,  0.5,  0.5}, {0,0}, {0,1,0}, {1,0,0}},

        {{ 0.5, -0.5, -0.5}, {0,1}, {1,0,0}, {0,0,1}},
        {{ 0.5, -0.5,  0.5}, {1,1}, {1,0,0}, {0,0,1}},
        {{ 0.5,  0.5,  0.5}, {1,0}, {1,0,0}, {0,0,1}},
        {{ 0.5,  0.5, -0.5}, {0,0}, {1,0,0}, {0,0,1}},

        {{-0.5, -0.5,  0.5}, {0,1}, {-1,0,0}, {0,0,-1}},
        {{-0.5, -0.5, -0.5}, {1,1}, {-1,0,0}, {0,0,-1}},
        {{-0.5,  0.5, -0.5}, {1,0}, {-1,0,0}, {0,0,-1}},
        {{-0.5,  0.5,  0.5}, {0,0}, {-1,0,0}, {0,0,-1}},

        {{ 0.5, -0.5,  0.5}, {0,1}, {0,0,1}, {-1,0,0}},
        {{-0.5, -0.5,  0.5}, {1,1}, {0,0,1}, {-1,0,0}},
        {{-0.5,  0.5,  0.5}, {1,0}, {0,0,1}, {-1,0,0}},
        {{ 0.5,  0.5,  0.5}, {0,0}, {0,0,1}, {-1,0,0}},

        {{-0.5, -0.5, -0.5}, {0,1}, {0,0,-1}, {1,0,0}},
        {{ 0.5, -0.5, -0.5}, {1,1}, {0,0,-1}, {1,0,0}},
        {{ 0.5,  0.5, -0.5}, {1,0}, {0,0,-1}, {1,0,0}},
        {{-0.5,  0.5, -0.5}, {0,0}, {0,0,-1}, {1,0,0}},
  };

  static const USHORT Indices[] = {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 10, 9, 8, 11, 10,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 22, 21, 20, 23, 22
  };

  static const std::array<VertexPos, 4> coloredPlaneVertices =
  {
       VertexPos{-1.0, -1.0, 0},
       VertexPos{-1.0, 1.0, 0},
       VertexPos{1.0, 1.0, 0},
       VertexPos{1.0, -1.0, 0},
  };

  static const std::array<USHORT, 6> coloredPlaneIndices =
  {
       0, 2, 1, 0, 3, 2,
  };
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
  if (SUCCEEDED(hr)) {
    SAFE_RELEASE(m_pDepthBuffer);
    SAFE_RELEASE(m_pDepthBufferDSV);
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Format = DXGI_FORMAT_D16_UNORM;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.Height = m_height;
    desc.Width = m_width;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    hr = m_pDevice->CreateTexture2D(&desc, NULL, &m_pDepthBuffer);
    if (SUCCEEDED(hr)) {
      hr = m_pDevice->CreateDepthStencilView(m_pDepthBuffer, NULL, &m_pDepthBufferDSV);
    }
    m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthBufferDSV);
  }
  return hr;
}

HRESULT Renderer::initScene(HWND hWnd) {
  HRESULT hr = S_OK;

  static const D3D11_INPUT_ELEMENT_DESC InputDesc[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}, };

  //cubes
  for (int i = 0; i < maxCubeNumber; i++) {
    CubeModel tmp;
    float textureIndex = (float)(rand() % 2);
    tmp.pos = XMFLOAT4((float)(rand() % 10 - 5), (float)(rand() % 10 - 5), (float)(rand() % 10 - 5), (float)(rand() % 6 - 3));
    tmp.shineSpeedIdNm = XMFLOAT4(300.0f, (float)(rand() % 5), textureIndex, textureIndex > 0.0f ? 0.0f : 1.0f);
    m_pCubeModelVector.push_back(tmp);
  }
  //vertex buffer
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
  //indices buffer
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
  D3DInclude includeObj;
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"vShader.hlsl", NULL, &includeObj, "main", "vs_5_0", flags, 0, &vShaderBuffer, NULL);
    hr = m_pDevice->CreateVertexShader(vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);
  }
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"pShader.hlsl", NULL, &includeObj, "main", "ps_5_0", flags, 0, &pShaderBuffer, NULL);
    hr = m_pDevice->CreatePixelShader(pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
  }
  if (SUCCEEDED(hr)) {
    int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
    hr = m_pDevice->CreateInputLayout(InputDesc, numElements, vShaderBuffer->GetBufferPointer(), vShaderBuffer->GetBufferSize(), &m_pInputLayout);
  }

  SAFE_RELEASE(vShaderBuffer);
  SAFE_RELEASE(pShaderBuffer);

  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(GeomMatrixBuffer) * maxCubeNumber;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    GeomMatrixBuffer geomMatrixBuffer[maxCubeNumber];
    for (int i = 0; i < m_pCubeModelVector.size(); i++) {
      geomMatrixBuffer[i].mWorldMatrix = XMMatrixTranslation(m_pCubeModelVector[i].pos.x, m_pCubeModelVector[i].pos.y, m_pCubeModelVector[i].pos.z);
      geomMatrixBuffer[i].norm = geomMatrixBuffer[i].mWorldMatrix;
      geomMatrixBuffer[i].shineSpeedTexIdNm = m_pCubeModelVector[i].shineSpeedIdNm;
    }

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &geomMatrixBuffer;
    data.SysMemPitch = sizeof(geomMatrixBuffer);
    data.SysMemSlicePitch = 0;



    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pGeomMatrixBuffer);
    assert(SUCCEEDED(hr));
  }
  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(SceneMatrixBuffer);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    hr = m_pDevice->CreateBuffer(&desc, nullptr, &m_pSceneMatrixBuffer);
    assert(SUCCEEDED(hr));
  }
  
  if (SUCCEEDED(hr)) {
    D3D11_RASTERIZER_DESC desc = {};
    desc.AntialiasedLineEnable = false;
    desc.FillMode = D3D11_FILL_SOLID;
    desc.CullMode = D3D11_CULL_BACK;
    desc.DepthBias = 0;
    desc.DepthBiasClamp = 0.0f;
    desc.FrontCounterClockwise = false;
    desc.DepthClipEnable = true;
    desc.ScissorEnable = false;
    desc.MultisampleEnable = false;
    desc.SlopeScaledDepthBias = 0.0f;

    hr = m_pDevice->CreateRasterizerState(&desc, &m_pRasterizerState);
    assert(SUCCEEDED(hr));
  }

  try {
    Texture tmp(m_pDevice, m_pDeviceContext, { L"data/kerenski.dds",  L"data/brick_diffuse.dds" });
    m_textureArray.push_back(tmp);
    m_textureArray.emplace_back(m_pDevice, m_pDeviceContext, L"data/brick_normal.dds");
    
    m_pLights = std::make_shared<Lights>();

    m_pLights->Add(
      {
        XMFLOAT4(0.0f, 1.5f, 3.0f, 0.0f),
        XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)
      });
    m_pLights->Add(
      {
        XMFLOAT4(0.5f, 1.5f, -3.0f, 0.0f),
        XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)
      });
    m_pLights->Add(
      {
        XMFLOAT4(-1.5f, 0.0f, -3.0f, 0.0f),
        XMFLOAT4(1.0f, 0.5f, 0.21f, 1.0f)
      });
    m_pLights->Add(
      {
        XMFLOAT4(0.0f, 0.0f, 3.0f, 0.0f),
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
      });
  }
  catch (...) {
    return E_FAIL;
  }

  m_pRenderTexture = std::make_shared<RenderTexture>(m_pDevice, m_width, m_height);
  m_pPostEffect = std::make_shared<PostEffect>(m_pDevice, hWnd, m_width, m_width);

  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(LightMatrixBuffer);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    LightMatrixBuffer lightMatrixBuffer;

    lightMatrixBuffer.lightCount.x = static_cast<int>(m_pLights->GetNumber());
    lightMatrixBuffer.lightCount.y = 1;
    lightMatrixBuffer.lightCount.z = 0;
    auto lightPositions = m_pLights->GetPositions();
    std::copy(lightPositions.begin(), lightPositions.end(), lightMatrixBuffer.lightPositions);
    auto lightColors = m_pLights->GetColors();
    std::copy(lightColors.begin(), lightColors.end(), lightMatrixBuffer.lightColors);

    lightMatrixBuffer.ambientColor = ambientColor_;


    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &lightMatrixBuffer;
    data.SysMemPitch = sizeof(lightMatrixBuffer);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pLightMatrixBuffer);
    assert(SUCCEEDED(hr));
  }


  if (SUCCEEDED(hr)) {
    D3D11_SAMPLER_DESC desc = {};

    desc.Filter = D3D11_FILTER_ANISOTROPIC;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.MinLOD = -D3D11_FLOAT32_MAX;
    desc.MaxLOD = D3D11_FLOAT32_MAX;
    desc.MipLODBias = 0.0f;
    desc.MaxAnisotropy = 16;
    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 1.0f;

    hr = m_pDevice->CreateSamplerState(&desc, &m_pSampler);
    assert(SUCCEEDED(hr));
  }
  //create depth state
  if (SUCCEEDED(hr)) {
    D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    dsDesc.StencilEnable = FALSE;

    hr = m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDepthState);
    assert(SUCCEEDED(hr));
  }
  
  // Create vertex buffer
  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(sizeof(VertexPos) * coloredPlaneVertices.size());
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = coloredPlaneVertices.data();
    data.SysMemPitch = static_cast<UINT>(sizeof(VertexPos) * coloredPlaneVertices.size());
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pTransparentVertexBuffer);
    assert(SUCCEEDED(hr));
  }
  //create index buffer
  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = static_cast<UINT>(sizeof(USHORT) * coloredPlaneIndices.size());
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = coloredPlaneIndices.data();
    data.SysMemPitch = static_cast<UINT>(sizeof(USHORT) * coloredPlaneIndices.size());
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, &data, &m_pTransparentIndexBuffer);
    assert(SUCCEEDED(hr));
  }

  //init shader
  ID3D10Blob* transVertexShaderBuffer = nullptr;
  ID3D10Blob* transPixelShaderBuffer = nullptr;

  int tflags = 0;
#ifdef _DEBUG
  tflags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
  D3D_SHADER_MACRO Shader_Macros[] = { {"USE_LIGHTS"}, {NULL, NULL} };
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"TransVertexShader.hlsl", NULL, &includeObj, "main", "vs_5_0", flags, 0, &transVertexShaderBuffer, NULL);
    hr = m_pDevice->CreateVertexShader(transVertexShaderBuffer->GetBufferPointer(), transVertexShaderBuffer->GetBufferSize(), NULL, &m_pTransparentVertexShader);
  }
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"TransPixelShader.hlsl", Shader_Macros, &includeObj, "main", "ps_5_0", flags, 0, &transPixelShaderBuffer, NULL);
    hr = m_pDevice->CreatePixelShader(transPixelShaderBuffer->GetBufferPointer(), transPixelShaderBuffer->GetBufferSize(), NULL, &m_pTransparentPixelShader);
  }
  if (SUCCEEDED(hr)) {
    int numElements = sizeof(InputDesc) / sizeof(InputDesc[0]);
    hr = m_pDevice->CreateInputLayout(InputDesc, numElements, transVertexShaderBuffer->GetBufferPointer(), transVertexShaderBuffer->GetBufferSize(), &m_pTransparentInputLayout);
  }

  SAFE_RELEASE(transVertexShaderBuffer);
  SAFE_RELEASE(transPixelShaderBuffer);

  //create const buffer
  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(TransparentWorldBuffer);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    TransparentWorldBuffer transMatrixBuffer;
    transMatrixBuffer.worldMatrix = DirectX::XMMatrixIdentity();

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &transMatrixBuffer;
    data.SysMemPitch = sizeof(transMatrixBuffer);
    data.SysMemSlicePitch = 0;

    hr = m_pDevice->CreateBuffer(&desc, NULL, &m_pTransparentWorldBuffer);
    if(SUCCEEDED(hr))
      hr = m_pDevice->CreateBuffer(&desc, NULL, &m_pTransparentWorldBuffer1);
    assert(SUCCEEDED(hr));
  }
  //create rasterizer state
  if (SUCCEEDED(hr)) {
    D3D11_RASTERIZER_DESC rasterizeDesc = {};
    rasterizeDesc.AntialiasedLineEnable = false;
    rasterizeDesc.FillMode = D3D11_FILL_SOLID;
    rasterizeDesc.CullMode = D3D11_CULL_NONE;
    rasterizeDesc.DepthBias = 0;
    rasterizeDesc.DepthBiasClamp = 0.0f;
    rasterizeDesc.FrontCounterClockwise = false;
    rasterizeDesc.DepthClipEnable = true;
    rasterizeDesc.ScissorEnable = false;
    rasterizeDesc.MultisampleEnable = false;
    rasterizeDesc.SlopeScaledDepthBias = 0.0f;

    hr = m_pDevice->CreateRasterizerState(&rasterizeDesc, &m_pTransparentRasterizerState);
    assert(SUCCEEDED(hr));
  }
  //create blend state
  if (SUCCEEDED(hr)) {
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].RenderTargetWriteMask =
      D3D11_COLOR_WRITE_ENABLE_RED |
      D3D11_COLOR_WRITE_ENABLE_GREEN |
      D3D11_COLOR_WRITE_ENABLE_BLUE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

    hr = m_pDevice->CreateBlendState(&blendDesc, &m_pTransparentBlendState);
    assert(SUCCEEDED(hr));
  }
  
  if (SUCCEEDED(hr)) {
    D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    dsDesc.StencilEnable = FALSE;
    hr = m_pDevice->CreateDepthStencilState(&dsDesc, &m_pTransparentDepthState);
    assert(SUCCEEDED(hr));
  }
  return hr;
}

bool Renderer::deviceInit(HINSTANCE hinst, HWND hWnd, Camera* pCamera, Input* pInput) {
  m_pCamera = pCamera;
  m_pInput = pInput;

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
    hr = initScene(hWnd);
  }

  SAFE_RELEASE(pSelectedAdapter);
  SAFE_RELEASE(pFactory);
  
  if (SUCCEEDED(hr)) {
    if(!m_pCamera)
      hr = S_FALSE;
  }

  if (SUCCEEDED(hr)) {
    if(!m_pInput)
      hr = S_FALSE;
  }
  m_pCubeMap = new CubeMap(m_pDevice, m_pDeviceContext, m_width, m_height, 10, 10);
  if (SUCCEEDED(hr)) {
    if(!m_pCubeMap)
      hr = S_FALSE;
  }

  if (FAILED(hr)) {
    deviceCleanup();
  }
  if (SUCCEEDED(hr)) {
    m_pFrustum = new Frustum;
    if(!m_pFrustum)
      hr = S_FALSE;
  }
  if(SUCCEEDED(hr))
    m_pFrustum->Init(0.1f);

  return SUCCEEDED(hr);
}

void Renderer::inputMovement() {
  XMFLOAT3 mouseMove = m_pInput->getMouseState();
  m_pCamera->getMouseState(mouseMove.x, mouseMove.y, mouseMove.z);
}

bool Renderer::getState() {
  HRESULT hr = S_OK;
  m_pCamera->getState();
  m_pInput->getState();

  inputMovement();

  static float t = 0.0f;
  static ULONGLONG timeStart = 0;
  ULONGLONG timeCur = GetTickCount64();
  if (timeStart == 0) {
    timeStart = timeCur;
  }
  t = (timeCur - timeStart) / 1000.0f;
  //std::size_t countSec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeCur;
  GeomMatrixBuffer gmb[maxCubeNumber];
  for (std::size_t i = 0; i < m_pCubeModelVector.size(); ++i) {
    gmb[i].mWorldMatrix = XMMatrixRotationX(t) * XMMatrixTranslation(m_pCubeModelVector[i].pos.x, m_pCubeModelVector[i].pos.y, m_pCubeModelVector[i].pos.z);
    gmb[i].norm = gmb[i].mWorldMatrix;
    gmb[i].shineSpeedTexIdNm = m_pCubeModelVector[i].shineSpeedIdNm;
  }
  m_pDeviceContext->UpdateSubresource(m_pGeomMatrixBuffer, 0, NULL, &gmb, 0, 0);

  TransparentWorldBuffer transparentWorldBuffer;
  transparentWorldBuffer.worldMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, -0.1f);
  transparentWorldBuffer.color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.5f, 0.5f);
  m_pDeviceContext->UpdateSubresource(m_pTransparentWorldBuffer, 0, NULL, &transparentWorldBuffer, 0, 0);

  transparentWorldBuffer.worldMatrix = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.1f);
  transparentWorldBuffer.color = DirectX::XMFLOAT4(0.0f, 0.6f, 1.0f, 0.5f);
  m_pDeviceContext->UpdateSubresource(m_pTransparentWorldBuffer1, 0, NULL, &transparentWorldBuffer, 0, 0);


  XMMATRIX mView;
  m_pCamera->getView(mView);

  XMMATRIX mProjection = XMMatrixPerspectiveFovLH(
      XM_PIDIV2, 
      m_width / (FLOAT)m_height, 
      100.0f, 0.01f);

  m_pFrustum->ConstructFrustum(mView, mProjection);

  m_cubeIndexies.clear();
  for (int i = 0; i < maxCubeNumber; i++) {
    XMFLOAT4 min, max;
    XMStoreFloat4(&min, XMVector4Transform(XMLoadFloat4(&AABB[0]), gmb[i].mWorldMatrix));
    XMStoreFloat4(&max, XMVector4Transform(XMLoadFloat4(&AABB[1]), gmb[i].mWorldMatrix));
    if (m_pFrustum->CheckRectangle(max.x, max.y, max.z, min.x, min.y, min.z)) {
      m_cubeIndexies.push_back(i);
    }
  }

  D3D11_MAPPED_SUBRESOURCE subresource;
  hr = m_pDeviceContext->Map(m_pSceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
  assert(SUCCEEDED(hr));
  const auto pov = m_pCamera->getPosition();
  if (SUCCEEDED(hr)) {
    SceneMatrixBuffer& sceneBuffer = *reinterpret_cast<SceneMatrixBuffer*>(subresource.pData);
    sceneBuffer.mViewProjectionMatrix = XMMatrixMultiply(mView, mProjection);
    
    for (int i = 0; i < m_cubeIndexies.size(); i++)
      sceneBuffer.indexBuffer[i] = XMINT4(m_cubeIndexies[i], 0, 0, 0);

    sceneBuffer.cameraPosition.x = pov.x;
    sceneBuffer.cameraPosition.y = pov.y;
    sceneBuffer.cameraPosition.z = pov.z;
    m_pDeviceContext->Unmap(m_pSceneMatrixBuffer, 0);
    //m_pDeviceContext->UpdateSubresource(m_pTransparentSceneBuffer, 0, NULL, &sceneBuffer, 0, 0);
  }

  m_pCubeMap->getState(m_pDeviceContext, mView, mProjection, pov);

  return SUCCEEDED(hr);
}

bool Renderer::render() {
  m_pDeviceContext->ClearState();

  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.Width = (FLOAT)m_width;
  vp.Height = (FLOAT)m_height;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;

  m_pDeviceContext->RSSetViewports(1, &vp);
  m_pDeviceContext->OMSetDepthStencilState(m_pDepthState, 0);

  D3D11_RECT rect;
  rect.left = 0;
  rect.right = m_width;
  rect.top = 0;
  rect.bottom = m_height;

  m_pDeviceContext->RSSetScissorRects(1, &rect);
  m_pRenderTexture->SetRenderTarget(m_pDeviceContext, m_pDepthBufferDSV);
  m_pRenderTexture->ClearRenderTarget(m_pDeviceContext, m_pDepthBufferDSV, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));


  m_pDeviceContext->RSSetState(m_pRasterizerState);
  m_pDeviceContext->OMSetDepthStencilState(m_pDepthState, 0);
  ID3D11SamplerState* samplers[] = { m_pSampler };
  m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampler);

  ID3D11ShaderResourceView* resources[] = { m_textureArray[0].GetTexture(), m_textureArray[1].GetTexture() };
  m_pDeviceContext->PSSetShaderResources(0, 2, resources);

  m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

  ID3D11Buffer* vBuffer1[] = {m_pVertexBuffer};
  UINT strides1[] = { sizeof(Vertex) };
  UINT offsets1[] = {0};

  m_pDeviceContext->IASetVertexBuffers(0, 1, vBuffer1, strides1, offsets1);
  m_pDeviceContext->IASetInputLayout(m_pInputLayout);
  m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
  m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pGeomMatrixBuffer);
  m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
  m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pGeomMatrixBuffer);
  m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);
  m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pLightMatrixBuffer);
  m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

  m_pDeviceContext->DrawIndexedInstanced(36, (UINT)m_cubeIndexies.size(), 0, 0, 0);

  m_pCubeMap->render(m_pDeviceContext);
  m_pDeviceContext->IASetIndexBuffer(m_pTransparentIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
  ID3D11Buffer* vertexBuffers[] = { m_pTransparentVertexBuffer };
  UINT strides[] = { 12 };
  UINT offsets[] = { 0 };
  m_pDeviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
  m_pDeviceContext->IASetInputLayout(m_pTransparentInputLayout);
  m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  m_pDeviceContext->RSSetState(m_pTransparentRasterizerState);
  m_pDeviceContext->VSSetShader(m_pTransparentVertexShader, NULL, 0);

  m_pDeviceContext->OMSetBlendState(m_pTransparentBlendState, NULL, 0xFFFFFFFF);
  m_pDeviceContext->OMSetDepthStencilState(m_pTransparentDepthState, 0);

  m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTransparentWorldBuffer);
  m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pTransparentWorldBuffer);
  m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pSceneMatrixBuffer);

  m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pLightMatrixBuffer);

  m_pDeviceContext->PSSetShader(m_pTransparentPixelShader, NULL, 0);
  m_pDeviceContext->DrawIndexed(static_cast<UINT>(coloredPlaneIndices.size()), 0, 0);

  m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pTransparentWorldBuffer1);
  m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pTransparentWorldBuffer1);
  m_pDeviceContext->DrawIndexed(static_cast<UINT>(coloredPlaneIndices.size()), 0, 0);

  ID3D11RenderTargetView* views[] = { m_pBackBufferRTV };
  m_pDeviceContext->OMSetRenderTargets(1, views, m_pDepthBufferDSV);

  static const FLOAT BackColor[4] = { 0.75f, 0.25f, 0.25f, 1.0f };
  m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, BackColor);
  m_pDeviceContext->ClearDepthStencilView(m_pDepthBufferDSV, D3D11_CLEAR_DEPTH, 0.0f, 0);

  m_pPostEffect->Process(m_pDeviceContext, m_pRenderTexture->GetSRV(), m_pBackBufferRTV, vp);

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

  SAFE_RELEASE(m_pRasterizerState);
  //SAFE_RELEASE(m_pWorldMatrixBuffer);
  //SAFE_RELEASE(m_pWorldMatrixBuffer1);
  SAFE_RELEASE(m_pSceneMatrixBuffer);
  SAFE_RELEASE(m_pSampler);

  SAFE_RELEASE(m_pDepthState);

  SAFE_RELEASE(m_pTransparentVertexShader);
  SAFE_RELEASE(m_pTransparentPixelShader);
  SAFE_RELEASE(m_pTransparentInputLayout);
  SAFE_RELEASE(m_pTransparentVertexBuffer);
  SAFE_RELEASE(m_pTransparentIndexBuffer);
  SAFE_RELEASE(m_pTransparentWorldBuffer);
  SAFE_RELEASE(m_pTransparentWorldBuffer1);
  //SAFE_RELEASE(m_pTransparentSceneBuffer);
  SAFE_RELEASE(m_pTransparentRasterizerState);
  SAFE_RELEASE(m_pTransparentDepthState);
  SAFE_RELEASE(m_pTransparentBlendState);

  SAFE_RELEASE(m_pDepthBuffer);
  SAFE_RELEASE(m_pDepthBufferDSV);
  SAFE_RELEASE(m_pLightMatrixBuffer);

  SAFE_RELEASE(m_pLightBuffer);
  SAFE_RELEASE(m_pGeomMatrixBuffer);
  SAFE_RELEASE(m_pFrustum);
  for (auto t : m_textureArray) {
    t.Release();
  }

  m_textureArray.clear();
  delete m_pCubeMap;
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
      m_pInput->resize(width, height);
      m_pCubeMap->resize(width, height);
    }
    return SUCCEEDED(hr);
  }
  return true;
}