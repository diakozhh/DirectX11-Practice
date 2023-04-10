#pragma once
#include <d3d11.h>
#include <directxmath.h>

class RenderTexture
{
public:
  RenderTexture(ID3D11Device* device, const unsigned width, const unsigned height);
  ~RenderTexture();
  bool resize(const unsigned width, const unsigned height);
  void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
  void ClearRenderTarget(
    ID3D11DeviceContext* deviceContext,
    ID3D11DepthStencilView* depthStencilView,
    const DirectX::XMFLOAT4& color);
  ID3D11Texture2D* GetRT();
  ID3D11RenderTargetView* GetRTV();
  ID3D11ShaderResourceView* GetSRV();
  D3D11_VIEWPORT GetViewPort();

private:
  ID3D11Device* m_pDevice;
  ID3D11Texture2D* m_pRTTexture;
  ID3D11RenderTargetView* m_pRTV;
  ID3D11ShaderResourceView* m_pSRV;
  D3D11_VIEWPORT m_viewport;
};