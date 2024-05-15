#include "sprite_batch.h"
#include "texture.h"
#include "shader.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>
//inline auto rotate(float& x, float& y, float cx, float cy, float angle);
inline auto rotate(float& x, float& y, float cx, float cy, float angle)
{
    x -= cx;
    y -= cy;
    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
    float tx{ x }, ty{ y };
    x = cos * tx + (-sin) * ty;
    y = sin * tx + cos * ty;
    x += cx;
    y += cy;
};

sprite_batch::sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites)
    : max_vertices(max_sprites * 6)
{
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, NULL, vertex_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };
    hr = create_vs_from_cso(device, "sprite_vs.cso",
        vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    hr = create_ps_from_cso(device, "sprite_ps.cso", pixel_shader[0].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    texture_size = { (float)texture2d_desc.Width,(float)texture2d_desc.Height };
}

sprite_batch::~sprite_batch()
{
    release_all_texture();
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context, float dx, float dy,
    float dw, float dh, float r, float g, float b, float a, float angle)
{
    render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, 0, 0, texture2d_desc.Width, texture2d_desc.Height);
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 rotate
)
{
    render(immediate_context, position.x, position.y, scale.x,scale.y, color.x, color.y, color.z, color.w, rotate.x, 0, 0, texture2d_desc.Width, texture2d_desc.Height);
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle/*degree*/,
    float sx, float sy, float sw, float sh)
{
    //スクリーン（ビューポート）のサイズを取得する
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);
    //renderメンバ関数の引数から矩形の各頂点の位置（スクリーン座標系）を計算する
    //(x0,y0)*----*(x1,y1)
    //       |   /|
    //       |  / |
    //       | /  |
    //       |/   |
    //(x2,y2)*----*(x3,y3)
    //left-top
    float x0{ dx };
    float y0{ dy };
    //right-top
    float x1{ dx + dw };
    float y1{ dy };
    //left-bottom
    float x2{ dx };
    float y2{ dy + dh };
    //right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };
    //点（x,y）が点(cx,cy）を中心に角（angle）で回転したときの座標を計算する関数オブジェクト（ラムダ式）
    //回転の中心を矩形の中心にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);
    //スクリーン座標系からNDCへの座標返還を行う
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;
    float u0{ sx / texture2d_desc.Width };
    float v0{ sy / texture2d_desc.Height };
    float u1{ (sx + sw) / texture2d_desc.Width };
    float v1{ (sy + sh) / texture2d_desc.Height };
    vertices.push_back({ {x0,y0,0},{r,g,b,a},{u0,v0} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x3,y3,0},{r,g,b,a},{u1,v1} });
}

void sprite_batch::begin(ID3D11DeviceContext* immediate_context, int pixel)
{
    vertices.clear();
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader[pixel].Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
}
void sprite_batch::end(ID3D11DeviceContext* immediate_context)
{
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    size_t vertex_count = vertices.size();
    _ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
    vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (data != nullptr)
    {
        const vertex* p = vertices.data();
        memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
    }
    immediate_context->Unmap(vertex_buffer.Get(), 0);
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());
    immediate_context->Draw(static_cast<UINT>(vertex_count), 0);
}