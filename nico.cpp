#include "nico.h"

Nico::Nico()
{
    position = { 0,0,-1 };
	coordinate_system = RHS_YUP;
	filename = ".\\resources\\nico.fbx";
}

Nico::~Nico()
{
}

void Nico::init()
{
}

void Nico::deinit()
{
}

void Nico::update()
{
}

void Nico::render()
{
}

void Nico::debug_imgui()
{
	if (ImGui::TreeNode("nico"))
	{

		if (ImGui::BeginMenu("color", true))
		{
			ImGui::ColorPicker3("color", &color.x);
			ImGui::SliderFloat("alpha", &color.w, 0.0f, 1.0f);
			ImGui::EndMenu();
		}
		ImGui::SliderFloat3("position", &position.x, -5, 5);
		ImGui::SliderFloat3("scale", &scale.x, 0.001f, 4.0f);
		ImGui::SliderFloat3("rotate", &rotate.x, -3.14f, 3.14f);
		ImGui::TreePop();
	}
}