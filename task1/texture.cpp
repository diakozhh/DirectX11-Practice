#include "texture.h"
#include <exception>

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* filename) {
  m_pTextureView = nullptr;

  HRESULT hr = S_OK;
  hr = DirectX::CreateDDSTextureFromFile(device, filename, nullptr, &m_pTextureView);
  if (FAILED(hr))
    throw std::exception("Failed to create texture");
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<const wchar_t*> filenames) {
  m_pTextureView = nullptr;

  HRESULT hr = S_OK;
  auto textureCount = (UINT)filenames.size();

  // A temporary collection to keep texture resources.
  std::vector<ID3D11Texture2D*> textures(textureCount);

  // Load textures from DDS files.
  for (UINT i = 0; i < textureCount; ++i) {
    hr = DirectX::CreateDDSTextureFromFile(device, filenames[i], (ID3D11Resource**)(&textures[i]), nullptr);
  }
  if (FAILED(hr)) {
    throw std::exception("Failed to create texture");
  }

  // Luna: Create the texture array.
  D3D11_TEXTURE2D_DESC textureDesc;
  textures[0]->GetDesc(&textureDesc); // each element in the texture array has the same format and dimensions

  D3D11_TEXTURE2D_DESC arrayDesc;
  arrayDesc.Width = textureDesc.Width;
  arrayDesc.Height = textureDesc.Height;
  arrayDesc.MipLevels = textureDesc.MipLevels;
  arrayDesc.ArraySize = textureCount;
  arrayDesc.Format = textureDesc.Format;
  arrayDesc.SampleDesc.Count = 1;
  arrayDesc.SampleDesc.Quality = 0;
  arrayDesc.Usage = D3D11_USAGE_DEFAULT;
  arrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  arrayDesc.CPUAccessFlags = 0;
  arrayDesc.MiscFlags = 0;

  ID3D11Texture2D* textureArray = nullptr;
  hr = device->CreateTexture2D(&arrayDesc, 0, &textureArray);
  if (FAILED(hr)) {
    throw std::exception("Failed to create texture");
  }

  // Copy each texture into the elements of the texture array.
  for (UINT texElement = 0; texElement < textureCount; ++texElement) {
    for (UINT mipLevel = 0; mipLevel < textureDesc.MipLevels; ++mipLevel) {
      const int sourceSubresource = D3D11CalcSubresource(mipLevel, 0, textureDesc.MipLevels);
      const int destSubresource = D3D11CalcSubresource(mipLevel, texElement, textureDesc.MipLevels);
      deviceContext->CopySubresourceRegion(textureArray, destSubresource, 0, 0, 0, textures[texElement], sourceSubresource, nullptr);
    }
  }

  // Luna: Create a resource view to the texture array.
  D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
  viewDesc.Format = arrayDesc.Format;
  viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
  viewDesc.Texture2DArray.MostDetailedMip = 0;
  viewDesc.Texture2DArray.MipLevels = arrayDesc.MipLevels;
  viewDesc.Texture2DArray.FirstArraySlice = 0;
  viewDesc.Texture2DArray.ArraySize = textureCount;

  hr = device->CreateShaderResourceView(textureArray, &viewDesc, &m_pTextureView);
  if (FAILED(hr)) {
    throw std::exception("Failed to create texture");
  }
  // Cleanup - we only need the resource view.
  textureArray->Release();
  for (UINT i = 0; i < textureCount; ++i) {
    textures[i]->Release();
  }

}

void Texture::Release() {
  if (m_pTextureView) {
    m_pTextureView->Release();
    m_pTextureView = nullptr;
  }
}
