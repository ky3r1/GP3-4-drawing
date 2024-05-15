#include "plantune.h"

Plantune::Plantune()
{
    position = { 0,0,-1 };
    scale_factor = 0.01f;
    coordinate_system = RHS_YUP;
    filename = ".\\resources\\plantune.fbx";
}

Plantune::~Plantune()
{
}

void Plantune::init()
{
}

void Plantune::deinit()
{
}

void Plantune::update()
{
}

void Plantune::render()
{
}

void Plantune::debug_imgui()
{
	if (ImGui::TreeNode("plantune"))
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
