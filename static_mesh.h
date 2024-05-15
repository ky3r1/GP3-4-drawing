#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <vector>
#include <fstream>
#include <filesystem>
#include "texture.h"

class static_mesh
{
public:
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };
    struct constants
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4 material_color;
    };

    //サブセットは対応するマテリアル名、そのマテリアルを使用するメッシュのインデックス開始番号とインデックス数
    struct subset
    {
        std::wstring usemtl;
        uint32_t index_start{ 0 };//start position of index buffer
        uint32_t index_count{ 0 };//number of vertices (indices)
    };
    std::vector<subset> subsets;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    //MTLファイルから取得するマテリアル情報構造体
    //std::wstring texture_filename;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    struct material
    {
        std::wstring name;
        DirectX::XMFLOAT4 Ka{ 0.2f,0.2f,0.2f,1.0f };
        DirectX::XMFLOAT4 Kd{ 0.8f,0.8f,0.8f,1.0f };
        DirectX::XMFLOAT4 Ks{ 1.0f,1.0f,1.0f,1.0f };
        std::wstring texture_filename[2];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view[2];
    };
    std::vector<material> materials;
public:
    static_mesh(ID3D11Device* device, const wchar_t* obj_filename);
    virtual ~static_mesh() = default;

    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);
    //テクスチャが存在しない時のダミーテクスチャ関数
    HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension);
protected:
    void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);
private:
    float texture_correction = 1.0f;
};