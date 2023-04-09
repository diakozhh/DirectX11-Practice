#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "utils.h"
#include <directxmath.h>

using namespace DirectX;

class PostEffect {
private:
  struct PostEffectConstantBuffer {
    XMINT4 params;
  };
public:
  PostEffect(ID3D11Device* device, HWND hwnd, const unsigned width, const unsigned height);
  ~PostEffect();
  // Render function
  void Process(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* renderTarget, D3D11_VIEWPORT viewport);
  // Switch flags functions
  void ToggleGrayScale(ID3D11DeviceContext* deviceContext);
private:
  ID3D11VertexShader* m_pVertexShader;
  ID3D11PixelShader* m_pPixelShader;
  ID3D11SamplerState* m_pSamplerState;
  ID3D11Buffer* m_pPostEffectConstantBuffer;

  unsigned width_;
  unsigned height_;

  // flag to turn on post effect
  bool m_isGrayScale = false;
};