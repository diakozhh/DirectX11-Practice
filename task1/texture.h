#pragma once

#include <d3d11.h>
#include <stdio.h>

#include "DDSTextureLoader.h"

class Texture {
private:
  ID3D11ShaderResourceView* m_pTextureView;
public:
  Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename);
  ID3D11ShaderResourceView* GetTexture() { return m_pTextureView; };
  void Release();
};