#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <iostream>
using namespace Microsoft::WRL;

class sprite
{
private:
    ComPtr<ID3D11VertexShader> vertex_shader;
    ComPtr<ID3D11PixelShader> pixel_shader;
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
        float sx,float sy,float sw,float sh);

    void render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,
        float dw, float dh
        );

    void textout(ID3D11DeviceContext* immediate_context, std::string s,
        float x, float y, float w, float h, float color[]);

    sprite(ID3D11Device* device, const wchar_t* filename);
    ~sprite();

    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };
};