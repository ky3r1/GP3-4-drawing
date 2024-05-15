#pragma once

#define SPRITE_BACK
#define NICO
//#define PLANTUNE

#include <d3d11.h>
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <wrl.h>

#include "misc.h"
#include "sprite_batch.h"
#include "geometric_primitive.h"
#include "high_resolution_timer.h"
#include "shader.h"
#include "skinned_mesh.h"
#include "framebuffer.h"
#include "fullscreen_quad.h"

#ifdef SPRITE_BACK
#include "sprite_back.h"
#endif // SPRITE_BACK

#ifdef NICO
#include "nico.h"
#endif // NICO

#ifdef PLANTUNE
#include "plantune.h"
#endif // PLANTUNE

#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

#include "nico.h"

using namespace Microsoft::WRL;

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"X3DGP" };

class framework
{
public:
	CONST HWND hwnd;


	framework(HWND hwnd);
	~framework();

	framework(const framework&) = delete;
	framework& operator=(const framework&) = delete;
	framework(framework&&) noexcept = delete;
	framework& operator=(framework&&) noexcept = delete;

	int run()
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.Get(), immediate_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 1
		BOOL fullscreen = 0;
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool uninitialize();

private:
	//DirectX11で利用する様々なリソースを作成する
	ComPtr<ID3D11Device> device;
	//様々な描画命令をGPUに伝える
	ComPtr<ID3D11DeviceContext> immediate_context;
	//前と後ろを入れ替える
	ComPtr<IDXGISwapChain> swap_chain;
	//色を書き込む
	ComPtr<ID3D11RenderTargetView> render_target_view;
	//奥行き情報を書き込む
	ComPtr<ID3D11DepthStencilView> depth_stencil_view;

	ComPtr<ID3D11SamplerState> sampler_states[3];
	int sampler_ = 0;

	ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
	int depth_ = 0;

	ComPtr<ID3D11BlendState> blend_states[7];
	int blend_ = 0;

	ComPtr<ID3D11RasterizerState> rasterizer_state[4];
	int rasterizer_ = 3;

	ComPtr<ID3D11PixelShader> pixcel_shaders[8];

	//std::unique_ptr<sprite> sprites[8];
	
	//sprite_batch* sprite_batches[8];
	std::unique_ptr<sprite_batch> sprite_batches[8];


	std::unique_ptr<geometric_primitive> geometric_primitives[8];

	std::unique_ptr<skinned_mesh> skinned_meshes[8];

	std::unique_ptr<framebuffer> framebuffers[8];

	std::unique_ptr<fullscreen_quad> bit_block_transfer;

	struct  scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection; //ビュー・プロジェクション変換行列
		DirectX::XMFLOAT4 light_direction; //ライトの向き
		DirectX::XMFLOAT4 camera_position; //カメラの位置
	};
	ComPtr<ID3D11Buffer> constant_buffers[8];

	high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };

	void calculate_frame_stats()
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
public:
	void render_3D(
		int index, 
		DirectX::XMFLOAT4X4 world,
		const DirectX::XMFLOAT4& material_color
	);

	void render_2D(
		int index,//番号
		DirectX::XMFLOAT3 position, //位置
		DirectX::XMFLOAT3 scale,//スケール
		DirectX::XMFLOAT4 color,//カラー
		DirectX::XMFLOAT3 rotate//回転
	);
private:
#ifdef SPRITE_BACK
	Sprite_Back sprite_back;
#endif // SPRITE_BACK

#ifdef NICO
	Nico nico;
#endif // NICO

#ifdef PLANTUNE
	Plantune plantune;
#endif // PLANTUNE
	float light_direction[4] = { 0,-1,1,0 };
	float camera_position[4] = { 0, 0,-10,1 };

	float animation_frame = { 0.0f };
};