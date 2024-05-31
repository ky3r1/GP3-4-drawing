#include "framework.h"

framework::framework(HWND hwnd) : hwnd(hwnd)
{
}

framework::~framework()
{

}

bool framework::initialize()
{
	//①デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };

	UINT create_device_flags{ 0 };
#ifdef _DEBUG
	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;
	swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = !FULLSCREEN;		//フルスクリーン
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
		&feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
		&swap_chain, &device, NULL, immediate_context.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//レンダーターゲットビューの作成
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();

	//深度ステンシルビューの作成
	ID3D11Texture2D* depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = SCREEN_WIDTH;
	texture2d_desc.Height = SCREEN_HEIGHT;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, NULL, &depth_stencil_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};

	depth_stencil_view_desc.Format = texture2d_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depth_stencil_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_buffer->Release();


	//ビューポートの設定

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);

	//サンプラーステートオブジェクトを生成する
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = device->CreateSamplerState(&sampler_desc, &sampler_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//深度ステンシルステートオブジェクトの作成
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = true;	//深度テストのon,off
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	//深度ステンシルバッファへの書き込みのon,off
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_desc.DepthEnable = true;	//深度テストのon,off
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	//深度ステンシルバッファへの書き込みのon,off
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_desc.DepthEnable = false;	//深度テストのon,off
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	//深度ステンシルバッファへの書き込みのon,off
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	depth_stencil_desc.DepthEnable = false;	//深度テストのon,off
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;	//深度ステンシルバッファへの書き込みのon,off
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_states[3]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//ブレンディングステートオブジェクトを生成する
	D3D11_BLEND_DESC blend_desc{};

	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = FALSE;

	//カラー
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//計算
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; //+
	//透明度
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = device->CreateBlendState(&blend_desc, &blend_states[1]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//alpha
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;

	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = device->CreateBlendState(&blend_desc, &blend_states[0]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//add
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;

	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;//1
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;//+
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;//src.a

	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;

	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, &blend_states[2]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//subtract
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, &blend_states[3]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//replace
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, &blend_states[4]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//darken
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, &blend_states[5]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//screen
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, &blend_states[6]);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//シーン定数バッファオブジェクトを生成する
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//geometric_primitives[0] = std::make_unique<geometric_primitive>(device.Get());
	//geometric_primitives[1] = std::make_unique<geometric_primitive>(device.Get());

	//rasterizerオブジェクト生成
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	//rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.FrontCounterClockwise = TRUE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//static_meshes[0] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\cube.obj");
	//static_meshes[1] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\torus.obj");
	//static_meshes[2] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Bison\\Bison.obj");
	//static_meshes[3] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Mr.Incredible\\Mr.Incredible.obj");
	//static_meshes[4] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Rock\\Rock.obj");
	//static_meshes[5] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\Cup\\Cup.obj");
	//static_meshes[6] = std::make_unique<static_mesh>(device.Get(), L".\\resources\\F-14A_Tomcat\\F-14A_Tomcat.obj");


	//skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\plantune.fbx", true,0);

	//skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\Drone166\\Drone166.1.fbx", true);

	//skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), ".\\resources\\AimTest\\MNK_Mesh.fbx", true);
	//skinned_meshes[0] ->append_animations(".\\resources\\AimTest\\Aim_Space.fbx", 0);

	//sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".¥¥resources¥¥screenshot.jpg", 1);

	//sprite_batches[0] = new sprite_batch(device.Get(), L".\\resources\\cyberpunk.jpg", 2048);

#ifdef SPRITE_BACK
	sprite_batches[0] = std::make_unique<sprite_batch>(device.Get(), L".\\resources\\screenshot.jpg", 1);
#endif // SPRITE_BACK

#ifdef NICO
	skinned_meshes[0] = std::make_unique<skinned_mesh>(device.Get(), nico.get_filename(), 0, true);
	//skinned_meshes[0] ->append_animations(nico.get_filename(), 0);
#endif // NICO

#ifdef PLANTUNE
	skinned_meshes[1] = std::make_unique<skinned_mesh>(device.Get(), plantune.get_filename(), 0, true);
#endif // PLANTUNE

	framebuffers[0] = std::make_unique<framebuffer>(device.Get(), 1280, 720);
	framebuffers[1] = std::make_unique<framebuffer>(device.Get(), 1280 / 2, 720 / 2);

	bit_block_transfer = std::make_unique<fullscreen_quad>(device.Get());
	create_ps_from_cso(device.Get(), "luminace_extraction_ps.cso", pixcel_shaders[0].GetAddressOf());
	create_ps_from_cso(device.Get(), "blur_ps.cso", pixcel_shaders[1].GetAddressOf());

	return true;
}



void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif


#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
	if (ImGui::TreeNode("sprite"))
	{
		const char* items_01[] = { "point_sampler_state", "linear_sampler_state", "anisotropic_sampler_state" };
		ImGui::Combo("Sampler", &sampler_, items_01, IM_ARRAYSIZE(items_01));
		const char* items_02[] = { "alpha", "add", "subtract","replace","multiply","lighten","darken"};
		ImGui::Combo("Blend", &blend_, items_02, IM_ARRAYSIZE(items_02));
		ImGui::SliderFloat4("light", light_direction, -1, 1);
		ImGui::TreePop();
	}
#ifdef SPRITE_BACK
	sprite_back.debug_imgui();
#endif // SPRITE_BACK

#ifdef NICO
	nico.debug_imgui();
#endif // NICO

#ifdef PLANTUNE
	plantune.debug_imgui();
#endif // PLANTUNE
	ImGui::End();
#endif
}

void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	ID3D11RenderTargetView * null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediate_context->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView * null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	immediate_context->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

	HRESULT hr{ S_OK };

	FLOAT color[]{ 0.2f,0.2f,0.2f,1.0f };		//画面の背景色設定
	//キャンバス全体を指定した色に塗りつぶす
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	//キャンバス全体の奥行き情報をリセットする
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//これから描くキャンバスを指定する
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());


	immediate_context->PSSetSamplers(0, 1, sampler_states[sampler_].GetAddressOf());
	immediate_context->PSSetSamplers(1, 1, sampler_states[sampler_].GetAddressOf());
	immediate_context->PSSetSamplers(2, 1, sampler_states[sampler_].GetAddressOf());

	immediate_context->OMSetBlendState(blend_states[blend_].Get(), nullptr, 0xFFFFFFFF);
	//ビュー・プロジェクション変換行列を計算し、それを定数バッファにセットする
	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	float aspect_ratio{ viewport.Width / viewport.Height };

	DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30),aspect_ratio,0.1f,100.0f) };
	DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f,0.0f,-10.0f,1.0f) };
	DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f,0.0f,0.0f,1.0f) };
	DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f) };
	DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye,focus,up) };

	scene_constants data{};
	DirectX::XMStoreFloat4x4(&data.view_projection, V * P);
	data.light_direction = { light_direction[0],light_direction[1] ,light_direction[2] ,light_direction[3] };
	data.camera_position = { camera_position[0],camera_position[1],camera_position[2],camera_position[3] };

	immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	//depth
	immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 1);
	//rasterizer
	immediate_context->RSSetState(rasterizer_state[3].Get());


	ID3D11ShaderResourceView* shader_resource_views[2]
	{
	framebuffers[0]->shader_resource_views[0].Get(),
	framebuffers[1]->shader_resource_views[0].Get()
	};
	bit_block_transfer->blit(immediate_context.Get(), shader_resource_views, 0, 2, pixcel_shaders[1].Get());
	framebuffers[1]->clear(immediate_context.Get());
	framebuffers[1]->activate(immediate_context.Get());
	bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixcel_shaders[0].Get());
	framebuffers[1]->deactivate(immediate_context.Get());
#if 1
	bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif
	framebuffers[0]->clear(immediate_context.Get());
	framebuffers[0]->activate(immediate_context.Get());

#ifdef SPRITE_BACK
	render_2D(0,
		sprite_back.get_position(),
		sprite_back.get_scale(),
		sprite_back.get_color(),
		sprite_back.get_rotate());
#endif // SPRITE_BACK

	// To chage the units from centimeters to meters, set 'scale_factor' to 0.01.
	int clip_index{ 0 };
	int frame_index{ 0 };
	static float animation_tick{ 0 };

	animation& animation{ skinned_meshes[0]->animation_clips.at(clip_index) };
	frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
	if (frame_index > animation.sequence.size() - 1)
	{
		frame_index = 0;
		animation_tick = 0;
	}
	else
	{
		animation_tick += elapsed_time;
	}
	animation::keyframe& keyframe{ animation.sequence.at(frame_index) };
	immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 1);
	immediate_context->RSSetState(rasterizer_state[0].Get());

#ifdef NICO
	skinned_meshes[0]->render(
		immediate_context.Get(),
		nico.get_world_transform(),
		nico.get_color(),
		& keyframe
	);
#endif
#ifdef PLANTUNE
	world = plantune.get_world_transform();
	skinned_meshes[1]->render(
		immediate_context.Get(),
		plantune.get_world_transform(),
		plantune.get_color(),
		&keyframe
		);
#endif // PLANTUNE

	framebuffers[0]->deactivate(immediate_context.Get());

#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	UINT sync_interval{ 0 };		//sync_intervalに1をセットすると60FPSの固定フレームレートで動作する

	swap_chain->Present(sync_interval, 0);
}

bool framework::uninitialize()
{
	return true;
}

void framework::render_3D(int index, DirectX::XMFLOAT4X4 world, const DirectX::XMFLOAT4& material_color)
{
	immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 1);
	immediate_context->RSSetState(rasterizer_state[0].Get());

	// To chage the units from centimeters to meters, set 'scale_factor' to 0.01.
	int clip_index{ 0 };
	int frame_index{ 0 };
	static float animation_tick{ 0 };

	animation& animation{ skinned_meshes[0]->animation_clips.at(clip_index) };
	frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
	if (frame_index > animation.sequence.size() - 1)
	{
		frame_index = 0;
		animation_tick = 0;
	}
	else
	{
		animation_tick += elapsed_time;
	}
	animation::keyframe& keyframe{ animation.sequence.at(frame_index) };

	//skinned_meshes[index]->render(
	//	immediate_context.Get(),
	//	world,
	//	material_color,
	//	&keyframe
	//);
}

void framework::render_2D(int index, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 rotate)
{
	//depth
	immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 1);
	//rasterizer
	immediate_context->RSSetState(rasterizer_state[3].Get());

	sprite_batches[index]->begin(immediate_context.Get(), index);
	sprite_batches[index]->render(immediate_context.Get(),
		position,
		scale,
		color,
		rotate);
	sprite_batches[index]->end(immediate_context.Get());
}
