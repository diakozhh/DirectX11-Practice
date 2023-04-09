#include "renderTexture.h"
#include "utils.h"
#include <exception>
#include <array>

RenderTexture::RenderTexture(ID3D11Device* device, const unsigned width, const unsigned height) :
  m_pDevice(device), m_pRTTexture(nullptr), m_pRTV(nullptr), m_pSRV(nullptr)
{
  if (!Resize(width, height))
    throw std::exception("Failed to resize render target");
}

RenderTexture::~RenderTexture()
{
  SAFE_RELEASE(m_pSRV);
  SAFE_RELEASE(m_pRTV);
  SAFE_RELEASE(m_pRTTexture);
}

bool RenderTexture::Resize(const unsigned width, const unsigned height)
{
  D3D11_TEXTURE2D_DESC textureDesc;
  ZeroMemory(&textureDesc, sizeof(textureDesc));

  textureDesc.Width = width;
  textureDesc.Height = height;
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  textureDesc.CPUAccessFlags = 0;
  textureDesc.MiscFlags = 0;

  HRESULT result = m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pRTTexture);
  if (FAILED(result))
    return false;

  D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
  renderTargetViewDesc.Format = textureDesc.Format;
  renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  renderTargetViewDesc.Texture2D.MipSlice = 0;

  result = m_pDevice->CreateRenderTargetView(m_pRTTexture, &renderTargetViewDesc, &m_pRTV);
  if (FAILED(result))
    return false;

  D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
  shaderResourceViewDesc.Format = textureDesc.Format;
  shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
  shaderResourceViewDesc.Texture2D.MipLevels = 1;

  result = m_pDevice->CreateShaderResourceView(m_pRTTexture, &shaderResourceViewDesc, &m_pSRV);
  if (FAILED(result))
    return false;

  m_viewport.Width = (FLOAT)width;
  m_viewport.Height = (FLOAT)height;
  m_viewport.MinDepth = 0.0f;
  m_viewport.MaxDepth = 1.0f;
  m_viewport.TopLeftX = 0;
  m_viewport.TopLeftY = 0;

  return true;
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
  deviceContext->OMSetRenderTargets(1, &m_pRTV, depthStencilView);
}

void RenderTexture::ClearRenderTarget(
  ID3D11DeviceContext* deviceContext,
  ID3D11DepthStencilView* depthStencilView,
  const DirectX::XMFLOAT4& color)
{
  std::array<float, 4> colorRaw = { color.x, color.y, color.z, color.w };
  deviceContext->ClearRenderTargetView(m_pRTV, colorRaw.data());
  deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 0.0f, 0);
}

ID3D11Texture2D* RenderTexture::GetRT()
{
  return m_pRTTexture;
}

ID3D11RenderTargetView* RenderTexture::GetRTV()
{
  return m_pRTV;
}

ID3D11ShaderResourceView* RenderTexture::GetSRV()
{
  return m_pSRV;
}

D3D11_VIEWPORT RenderTexture::GetViewPort()
{
  return m_viewport;
}