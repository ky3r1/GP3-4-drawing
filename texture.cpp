#include "texture.h"

#include <WICTextureLoader.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>

#include <fstream>
#include <iostream> 
#include <filesystem>
#include <string> 
#include <DDSTextureLoader.h>


using namespace std;

static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;
HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename,
    ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
    HRESULT hr{ S_OK };
    ComPtr<ID3D11Resource> resource;

    auto it = resources.find(filename);
    if (it != resources.end())
    {
        *shader_resource_view = it->second.Get();
        (*shader_resource_view)->AddRef();
        (*shader_resource_view)->GetResource(resource.GetAddressOf());
    }
    else
    {
#if false
        hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        resources.insert(make_pair(filename, *shader_resource_view));
#else
        std::filesystem::path dds_filename(filename);
        dds_filename.replace_extension("dds");
        if (std::filesystem::exists(dds_filename.c_str()))
        {
            hr = CreateDDSTextureFromFile(device, dds_filename.c_str(), resource.GetAddressOf(), shader_resource_view);
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }
        else
        {

            hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);

            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }
        resources.insert(make_pair(filename, *shader_resource_view));
#endif
    }

    ComPtr<ID3D11Texture2D> texture2d;
    hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    texture2d->GetDesc(texture2d_desc);

    return hr;
}
void release_all_texture()
{
    resources.clear();
}