#include "sprite.h"
#include "texture.h"
#include "shader.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>


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

sprite::sprite(ID3D11Device* device, const wchar_t* filename)
{
    //�@���_���̃Z�b�g
    vertex vertices[]
    {
        {{-1.0,+1.0,0},{1,1,1,1},{0,0}},
        {{+1.0,+1.0,0},{1,0,0,1},{1,0}},
        {{-1.0,-1.0,0},{0,1,0,1},{0,1}},
        {{+1.0,-1.0,0},{0,0,0,1},{1,1}},
    };

    //�A���_�o�b�t�@�I�u�W�F�N�g�̐���
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(vertices);
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    //HRESULT hr{ S_OK };
    HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    //�B���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    //const char* cso_name{ "sprite_vs.cso" };

    //FILE* fp{};
    //fopen_s(&fp, cso_name, "rb");
    //_ASSERT_EXPR_A(fp, "CSO File not found");

    //fseek(fp, 0, SEEK_END);
    //long cso_sz{ ftell(fp) };
    //fseek(fp, 0, SEEK_SET);

    //std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
    //fread(cso_data.get(), cso_sz, 1, fp);
    //fclose(fp);

    ////HRESULT hr{ S_OK };
    //hr={ S_OK };
    //hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
    //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    //�C���̓��C�A�E�g�I�u�W�F�N�g�̍쐬
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };
    /*hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
        cso_data.get(), cso_sz, &input_layout);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));*/


    hr = create_vs_from_cso(device, "sprite_vs.cso",
        vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    ////�摜�t�@�C���̃��[�h�ƃV�F�[�_�[���\�[�X�r���[�I�u�W�F�N�g�iID3D11ShaderResourceView)�̐���
    //ID3D11Resource* resource{};
    //hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view);
    //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //resource->Release();

    ////�e�N�X�`�����iD3D11TEXTURE2D_DESC�j�̎擾
    //ID3D11Texture2D* texture2d{};
    //hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
    //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    //texture2d->GetDesc(&texture2d_desc);
    //texture2d->Release();

    hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    texture_size = { (float)texture2d_desc.Width,(float)texture2d_desc.Height };


    //�D�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    //cso_name = { "sprite_ps.cso" };

    //fp = {};
    //fopen_s(&fp, cso_name, "rb");
    //_ASSERT_EXPR_A(fp, "CSO File not found");

    //fseek(fp, 0, SEEK_END);
    //cso_sz = { ftell(fp) };
    //fseek(fp, 0, SEEK_SET);

    //cso_data = { std::make_unique<unsigned char[]>(cso_sz) };
    //fread(cso_data.get(), cso_sz, 1, fp);
    //fclose(fp);

    ////HRESULT hr{ S_OK };Enemy* enemy1 = enemyManager.GetEnemy(i);
    //hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader);
    //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    hr = create_ps_from_cso(device, "sprite_ps.cso", pixel_shader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void sprite::render(ID3D11DeviceContext* immediate_context, float dx, float dy,
    float dw, float dh, float r, float g, float b, float a,float angle)
{
//    //�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
//    D3D11_VIEWPORT viewport{};
//    UINT num_viewports{ 1 };
//    immediate_context->RSGetViewports(&num_viewports, &viewport);
//
//    //render�����o�֐��̈��������`�̊e���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����
//
//    //(x0,y0)*----*(x1,y1)
//    //       |   /|
//    //       |  / |
//    //       | /  |
//    //       |/   |
//    //(x2,y2)*----*(x3,y3)
//
//    //left-top
//    float x0{ dx };
//    float y0{ dy };
//    //right-top
//    float x1{ dx + dw };
//    float y1{ dy };
//    //left-bottom
//    float x2{ dx };
//    float y2{ dy + dh };
//    //right-bottom
//    float x3{ dx + dw };
//    float y3{ dy + dh };
//
//    //�_�ix,y�j���_(cx,cy�j�𒆐S�Ɋp�iangle�j�ŉ�]�����Ƃ��̍��W���v�Z����֐��I�u�W�F�N�g�i�����_���j
//   
//    //��]�̒��S����`�̒��S�ɂ����ꍇ
//    float cx = dx + dw * 0.5f;
//    float cy = dy + dh * 0.5f;
//
//    rotate(x0, y0, cx, cy, angle);
//    rotate(x1, y1, cx, cy, angle);
//    rotate(x2, y2, cx, cy, angle);
//    rotate(x3, y3, cx, cy, angle);
//
//    //�X�N���[�����W�n����NDC�ւ̍��W�Ԋ҂��s��
//    x0 = 2.0f * x0 / viewport.Width - 1.0f;
//    y0 = 1.0f - 2.0f * y0 / viewport.Height;
//    x1 = 2.0f * x1 / viewport.Width - 1.0f;
//    y1 = 1.0f - 2.0f * y1 / viewport.Height;
//    x2 = 2.0f * x2 / viewport.Width - 1.0f;
//    y2 = 1.0f - 2.0f * y2 / viewport.Height;
//    x3 = 2.0f * x3 / viewport.Width - 1.0f;
//    y3 = 1.0f - 2.0f * y3 / viewport.Height;
//
//    //�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g
//    HRESULT hr{ S_OK };
//    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
//    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
//    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
//    if (vertices != nullptr)
//    {
//        vertices[0].position = { x0,y0,0 };
//        vertices[1].position = { x1,y1,0 };
//        vertices[2].position = { x2,y2,0 };
//        vertices[3].position = { x3,y3,0 };
//#if 1
//        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };
//#else
//        vertices[0].color = { r,0,0,a };
//        vertices[1].color = { 0,g,0,a };
//        vertices[2].color = { 0,0,b,a };
//        vertices[3].color = { r,g,b,a };
//#endif
//
//        vertices[0].texcoord = { 0,0 };
//        vertices[1].texcoord = { 1,0 };
//        vertices[2].texcoord = { 0,1 };
//        vertices[3].texcoord = { 1,1 };
//    }
//
//    immediate_context->Unmap(vertex_buffer.Get(), 0);
//
//
//    //�@���_�o�b�t�@�[�̃o�C���h
//    UINT stride{ sizeof(vertex) };
//    UINT offset{ 0 };
//
//
//    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
//
//    //�A�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
//    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//    //�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
//    immediate_context->IASetInputLayout(input_layout.Get());
//
//    //�C�V�F�[�_�[�̃o�C���h
//    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
//    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
//
//    //�V�F�[�_�[���\�[�X�̃o�C���h
//    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
//
//    //�D�v���~�e�B�u�̕`��
//    immediate_context->Draw(4, 0);
//render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, texture2d_desc.Width, texture2d_desc.Height);
}


void sprite::render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle/*degree*/,
    float sx, float sy, float sw, float sh)
{
    //�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    //render�����o�֐��̈��������`�̊e���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����

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

    //�_�ix,y�j���_(cx,cy�j�𒆐S�Ɋp�iangle�j�ŉ�]�����Ƃ��̍��W���v�Z����֐��I�u�W�F�N�g�i�����_���j
  
    //��]�̒��S����`�̒��S�ɂ����ꍇ
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;

    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    //�X�N���[�����W�n����NDC�ւ̍��W�Ԋ҂��s��
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    float texLeft = sx;
    float texTop = sy;
    float texRight = sx + sw;
    float texBottom = sy + sh;



    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0,y0,0 };
        vertices[1].position = { x1,y1,0 };
        vertices[2].position = { x2,y2,0 };
        vertices[3].position = { x3,y3,0 };
#if 1
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };
#else
        vertices[0].color = { r,g,b,a };
        vertices[1].color = { r,g,b,a };
        vertices[2].color = { r,g,b,a };
        vertices[3].color = { r,g,b,a };
#endif

        vertices[0].texcoord = { texLeft / texture_size.x,texTop / texture_size.y };
        vertices[1].texcoord = { texRight / texture_size.x,texTop / texture_size.y };
        vertices[2].texcoord = { texLeft / texture_size.x,texBottom / texture_size.y };
        vertices[3].texcoord = { texRight / texture_size.x,texBottom / texture_size.y };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);


    //�@���_�o�b�t�@�[�̃o�C���h
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };


    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    //�A�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
    immediate_context->IASetInputLayout(input_layout.Get());

    //�C�V�F�[�_�[�̃o�C���h
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //�V�F�[�_�[���\�[�X�̃o�C���h
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    //�D�v���~�e�B�u�̕`��
    immediate_context->Draw(4, 0);
}

void sprite::render(ID3D11DeviceContext* immediate_context,
    //�C���^�[�t�F�C�X��ύX����
    float dx, float dy, //����̍��W
    float dw, float dh  //�T�C�Y
)
{
    //�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    //render�����o�֐��̈��������`�̊e���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����

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

    //�_�ix,y�j���_(cx,cy�j�𒆐S�Ɋp�iangle�j�ŉ�]�����Ƃ��̍��W���v�Z����֐��I�u�W�F�N�g�i�����_���j

    //��]�̒��S����`�̒��S�ɂ����ꍇ
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;

    rotate(x0, y0, cx, cy, 0);
    rotate(x1, y1, cx, cy, 0);
    rotate(x2, y2, cx, cy, 0);
    rotate(x3, y3, cx, cy, 0);

    //�X�N���[�����W�n����NDC�ւ̍��W�Ԋ҂��s��
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));




    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0,y0,0 };
        vertices[1].position = { x1,y1,0 };
        vertices[2].position = { x2,y2,0 };
        vertices[3].position = { x3,y3,0 };
#if 1
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1,1,1,1 };
#else
        vertices[0].color = { r,g,b,a };
        vertices[1].color = { r,g,b,a };
        vertices[2].color = { r,g,b,a };
        vertices[3].color = { r,g,b,a };
#endif

        vertices[0].texcoord = { 0,0 };
        vertices[1].texcoord = { 1,0 };
        vertices[2].texcoord = { 0,1 };
        vertices[3].texcoord = { 1,1 };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);


    //�@���_�o�b�t�@�[�̃o�C���h
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };


    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    //�A�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
    immediate_context->IASetInputLayout(input_layout.Get());

    //�C�V�F�[�_�[�̃o�C���h
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //�V�F�[�_�[���\�[�X�̃o�C���h
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    //�D�v���~�e�B�u�̕`��
    immediate_context->Draw(4, 0);
}

void sprite::textout(ID3D11DeviceContext* immediate_context, std::string s,
    float x, float y, float w, float h, float color[])
{
    float sw = static_cast<float>(texture2d_desc.Width / 16);
    float sh = static_cast<float>(texture2d_desc.Height / 16);
    float carriage = 0;
    for (const char c : s)
    {
        render(immediate_context, x + carriage, y, w, h, color[0], color[1], color[2], color[3], 0,
            sw * (c & 0x0F), sh * (c >> 4), sw, sh);
        carriage += w;
    }
}

sprite::~sprite()
{
    release_all_texture();
    /*vertex_shader->Release();
    pixel_shader->Release();
    input_layout->Release();
    vertex_buffer->Release();*/
    //shader_resource_view->Release();
}
