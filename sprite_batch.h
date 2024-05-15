#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl.h>
using namespace Microsoft::WRL;


class sprite_batch
{
private:
    ComPtr<ID3D11VertexShader> vertex_shader;
    ComPtr<ID3D11PixelShader> pixel_shader[2];
    ComPtr<ID3D11InputLayout> input_layout;
    ComPtr<ID3D11Buffer> vertex_buffer;

    ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    D3D11_TEXTURE2D_DESC texture2d_desc;

    DirectX::XMFLOAT2 texture_size;

public:
    void render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,
        float dw, float dh,
        float r, float g, float b, float a,
        float angle/*degree*/);

    void render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,
        float dw, float dh,
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);

    void render(ID3D11DeviceContext* immediate_context,
        DirectX::XMFLOAT3 position,//position
        DirectX::XMFLOAT3 scale,//scale
        DirectX::XMFLOAT4 color,//color
        DirectX::XMFLOAT3 rotate//rotate
    );

    sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
    ~sprite_batch();

    void begin(ID3D11DeviceContext* immediate_context,int pixel);
    void end(ID3D11DeviceContext* immediate_context);

    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    const size_t max_vertices;
    std::vector<vertex> vertices;
};