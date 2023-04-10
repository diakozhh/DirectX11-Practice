#include "postEffect.h"
#include <exception>

PostEffect::PostEffect(ID3D11Device* device, HWND hwnd, const unsigned width, const unsigned height) :
  width_(width),
  height_(height),

  m_pVertexShader(nullptr),
  m_pPixelShader(nullptr),
  m_pSamplerState(nullptr),
  m_pPostEffectConstantBuffer(nullptr)
{
  HRESULT hr = S_OK;

  ID3D10Blob* vertexShaderBuffer = nullptr;
  ID3D10Blob* pixelShaderBuffer = nullptr;
  int flags = 0;
#ifdef _DEBUG
  flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
  // Compile the vertex shader code.
  hr = D3DCompileFromFile(L"postEffectVShader.hlsl", NULL, NULL, "main", "vs_5_0", flags, 0, &vertexShaderBuffer, NULL);
  hr = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader);

  // Compile the pixel shader code.
  if (SUCCEEDED(hr)) {
    hr = D3DCompileFromFile(L"postEffectPShader.hlsl", NULL, NULL, "main", "ps_5_0", flags, 0, &pixelShaderBuffer, NULL);
    hr = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader);
  }
  if (FAILED(hr))
    throw std::exception("Failed to compile shader");

  // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
  SAFE_RELEASE(vertexShaderBuffer);
  SAFE_RELEASE(pixelShaderBuffer);

  if (SUCCEEDED(hr)) {
    // Create the sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

    // Create the texture sampler state.
    hr = device->CreateSamplerState(&samplerDesc, &m_pSamplerState);
    if (FAILED(hr))
      throw std::exception("Failed to create sampler state");
  }

  // Create constant bufer
  if (SUCCEEDED(hr)) {
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(PostEffectConstantBuffer);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    PostEffectConstantBuffer postEffectConstantBuffer;
    postEffectConstantBuffer.params = XMINT4(m_isGrayScale, 0, 0, 0);

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &postEffectConstantBuffer;
    data.SysMemPitch = sizeof(postEffectConstantBuffer);
    data.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&desc, &data, &m_pPostEffectConstantBuffer);
    if (FAILED(hr))
      throw std::exception("Failed to create constant buffer");
  }
}

PostEffect::~PostEffect() {
  SAFE_RELEASE(m_pSamplerState);
  SAFE_RELEASE(m_pPixelShader);
  SAFE_RELEASE(m_pVertexShader);
  SAFE_RELEASE(m_pPostEffectConstantBuffer);
}

void PostEffect::Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport) {
  deviceContext->OMSetRenderTargets(1, &renderTarget, nullptr);
  deviceContext->RSSetViewports(1, &viewport);

  deviceContext->IASetInputLayout(nullptr);
  deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  deviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
  deviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
  deviceContext->PSSetConstantBuffers(0, 1, &m_pPostEffectConstantBuffer);
  deviceContext->PSSetShaderResources(0, 1, &sourceTexture);
  deviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

  deviceContext->Draw(3, 0);

  ID3D11ShaderResourceView* nullsrv[] = { nullptr };
  deviceContext->PSSetShaderResources(0, 1, nullsrv);
}

// Switch flags functions
void PostEffect::ToggleGrayScale(ID3D11DeviceContext* deviceContext) {
  m_isGrayScale = !m_isGrayScale;
  PostEffectConstantBuffer postEffectConstantBuffer;
  postEffectConstantBuffer.params = XMINT4(m_isGrayScale, 0, 0, 0);
  deviceContext->UpdateSubresource(m_pPostEffectConstantBuffer, 0, nullptr, &postEffectConstantBuffer, 0, 0);
}