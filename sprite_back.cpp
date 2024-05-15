#include "sprite_back.h"

Sprite_Back::Sprite_Back()
{
    scale = { 1280.0f, 720.0f,1.0f };
	filename = ".\\resources\\screenshot.jpg";
}

Sprite_Back::~Sprite_Back()
{
}

void Sprite_Back::init()
{
}

void Sprite_Back::deinit()
{
}

void Sprite_Back::update()
{
}

void Sprite_Back::render()
{
}

void Sprite_Back::debug_imgui()
{
	if (ImGui::TreeNode("sprite_back"))
	{

		if (ImGui::BeginMenu("color", true))
		{
			ImGui::ColorPicker3("color", &color.x);
			ImGui::SliderFloat("alpha", &color.w, 0.0f, 1.0f);
			ImGui::EndMenu();
		}
		ImGui::SliderFloat3("position", &position.x, -720*0.5, 720*0.5);
		ImGui::SliderFloat3("scale", &scale.x, 0.001f, 1280.0f);
		ImGui::SliderFloat3("rotate", &rotate.x, -180.0f, 180.0f);
		ImGui::TreePop();
	}
}
