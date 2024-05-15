#include "geometric_primitive.h"
#include "shader.h"

geometric_primitive::geometric_primitive(ID3D11Device* device)
{
    vertex vertices_cube[24]{
        //front
        {{0.5f,0.5f,-0.5f},{0.0f,0.0f,-1.0f}}, //0
        {{0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f}}, //1
        {{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f}}, //2
        {{-0.5f,0.5f,-0.5f},{0.0f,0.0f,-1.0f}}, //3
        //back
        {{-0.5f,0.5f,0.5f},{0.0f,0.0f,1.0f}}, //4
        {{-0.5f,-0.5f,0.5f},{0.0f,0.0f,1.0f}}, //5
        {{0.5f,-0.5f,0.5f},{0.0f,0.0f,1.0f}}, //6
        {{0.5f,0.5f,0.5f},{0.0f,0.0f,1.0f}}, //7
        //top
        {{-0.5f,0.5f,0.5f},{0.0f,1.0f,0.0f}}, //8
        {{0.5f,0.5f,0.5f},{0.0f,1.0f,0.0f}}, //9
        {{0.5f,0.5f,-0.5f},{0.0f,1.0f,0.0f}}, //10
        {{-0.5f,0.5f,-0.5f},{0.0f,1.0f,0.0f}}, //11
        //bottom
        {{0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f}}, //12
        {{0.5f,-0.5f,0.5f},{0.0f,-1.0f,0.0f}}, //13
        {{-0.5f,-0.5f,0.5f},{0.0f,-1.0f,0.0f}}, //14
        {{-0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f}}, //15
        //left
        {{-0.5f,0.5f,-0.5f},{-1.0f,0.0f,0.0f}}, //16
        {{-0.5f,-0.5f,-0.5f},{-1.0f,0.0f,0.0f}}, //17
        {{-0.5f,-0.5f,0.5f},{-1.0f,0.0f,0.0f}}, //18
        {{-0.5f,0.5f,0.5f},{-1.0f,0.0f,0.0f}}, //19
        //right
        {{0.5f,0.5f,-0.5f},{-1.0f,0.0f,0.0f}},  //20
        {{0.5f,0.5f,0.5f},{-1.0f,0.0f,0.0f}},  //21
        {{0.5f,-0.5f,0.5f},{-1.0f,0.0f,0.0f}},  //22
        {{0.5f,-0.5f,-0.5f},{-1.0f,0.0f,0.0f}},  //23
    };

    uint32_t indices_cube[36]{
        //front
        0,1,2,
        0,2,3,
        //back
        4,5,6,
        4,6,7,
        //top
        8,9,10,
        8,10,11,
        //bottom
        12,13,14,
        12,14,15,
        //left
        16,17,18,
        16,18,19,
        //right
        20,21,22,
        20,22,23,
    };

    create_com_buffers(device, vertices_cube, 24, indices_cube, 36);

    HRESULT hr{ S_OK };

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };
    create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
        input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void geometric_primitive::render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    constants data{ world,material_color };
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    index_buffer->GetDesc(&buffer_desc);
    immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

void geometric_primitive::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
    uint32_t* indices, size_t index_count)
{
    HRESULT hr{ S_OK };

    D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = indices;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

