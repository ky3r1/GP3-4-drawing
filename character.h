#pragma once
#include <d3d11.h>
#include <windows.h>
#include <DirectXMath.h>



#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

class Character
{
public:
	//コンストラクタ
    Character() {};
	//デストラクタ
	virtual ~Character() {};


    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void update() = 0;
    virtual void render() = 0;

	virtual void debug_imgui() = 0;


	//position
	const DirectX::XMFLOAT3& get_position()  const { return position; }
	void set_position(DirectX::XMFLOAT3 p) { position = p; }

	//scale
	const DirectX::XMFLOAT3& get_scale()  const { return scale; }
	void set_scale(DirectX::XMFLOAT3 s) { scale = s; }

	//rotate
	const DirectX::XMFLOAT3& get_rotate()  const { return rotate; }
	void set_rotate(DirectX::XMFLOAT3 r) { rotate = r; }

	//color
	const DirectX::XMFLOAT4& get_color()  const { return color; }
	void set_position(DirectX::XMFLOAT4 c) { color = c; }

	//transform
	const DirectX::XMFLOAT4X4& get_transform()  const { return transform; }
	void set_position(DirectX::XMFLOAT4X4 t) { transform = t; }

	//scale_factor
	const float get_scale_factor()const { return scale_factor; }

	//world_transform
	DirectX::XMFLOAT4X4 get_world_transform();

	//filename
	const char* get_filename() const { return filename; }

protected:
	float scale_factor = 0.01;
	DirectX::XMFLOAT3 position = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 scale= { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 rotate = { 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
	DirectX::XMFLOAT4X4 transform =
	{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	int coordinate_system;

	char* filename;
private:
	//座標系
	DirectX::XMFLOAT4X4 coordinate_system_transforms[4]
	{
		{-1,0,0,0
		 ,0,1,0,0
		 ,0,0,1,0
		 ,0,0,0,1},//0:RHS Y-UP
		{ 1,0,0,0
		 ,0,1,0,0
		 ,0,0,1,0
		 ,0,0,0,1},//1:LHS Y-UP
		{-1,0,0,0
		 ,0,0,-1,0
		 ,0,1,0,0
		 ,0,0,0,1},//2:RHS Z-UP
		{ 1,0,0,0
		 ,0,0,1,0
		 ,0,1,0,0
		 ,0,0,0,1},//3:LHS Z-UP
	};
};

enum
{
	RHS_YUP,
	LHS_YUP,
	RHS_ZUP,
	LHS_ZUP
};