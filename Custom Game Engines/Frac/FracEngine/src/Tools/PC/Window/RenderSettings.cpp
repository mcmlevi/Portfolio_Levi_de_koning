#include "pch.h"
#include "Tools/PC/Window/RenderSettings.h"

#include <DearImGui\imgui.h>

namespace Frac
{
	RenderSettings::RenderSettings() : 
		m_attenuation(0),
		m_debugPass(0),
		m_blit(false),
		m_hdr(false),
		m_toneMapping(0),
		m_exposure(0.0f),
		m_maxWhite(0.0f),
		m_materialChanges(0),
		m_meshChanges(0)
	{

	}

	RenderSettings::~RenderSettings()
	{

	}

	void RenderSettings::Update()
	{
		int imguiID = 0;
		if (m_windowState)
		{
			ImGui::Begin("Render Settings", nullptr, ImGuiWindowFlags_NoCollapse);

			{ // Reload shader functionality
				std::string shaderSource = "[Shader file path]"; // TODO get shader path from resource manager
				char shaderSourceTextBuffer[256]{};
				for (int i = 0; i < shaderSource.size(); i++)
				{
					shaderSourceTextBuffer[i] = shaderSource[i];
				}

				ImGui::PushID(imguiID++);
				ImGui::Button("Reload");
				if (ImGui::IsItemClicked(0))
				{
					//Reload shader
				}
				ImGui::SameLine();
				ImGui::PushItemWidth(500);
				ImGui::InputText("", shaderSourceTextBuffer, sizeof(shaderSourceTextBuffer), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopID();
			}

			{ // Alter shader uniforms | TODO Reflection to automate the process of reading out uniforms
				int* a = (int*)&m_attenuation; // TODO get attenuation reference from renderer
				const char* algs[] = { "GLTF", "Blast", "Unreal", "Smoothstep" };
				ImGui::Combo("Attenuation", a, algs, 4);

				const char* passes[] = { "None", "Position", "Normal", "Color" , "Depth" };
				ImGui::Combo("Debug G-buffer", &m_debugPass, passes, 5); // TODO get Debug Pass reference from renderer

				ImGui::Checkbox("Blit", &m_blit); // TODO get Blit reference from renderer
				ImGui::Checkbox("HDR", &m_hdr); // TODO get HDR reference from renderer

				int* t = (int*)&m_toneMapping; // TODO get Tone Mapping reference from renderer
				const char* toneAlgs[] = { "Linear", "Reinhard", "Reinhard Extended", "Reinhard Extended Luminance", "Reinhard-Jodie", "Uncharted 2", "ACES Aprox" };
				ImGui::Combo("Tone Mapping", t, toneAlgs, 7);
				ImGui::DragFloat("Exposure", &m_exposure, 0.01f); // TODO get Exposure reference from renderer
				ImGui::DragFloat("Max White", &m_maxWhite, 0.01f); // TODO get Max white reference from renderer

				ImGui::LabelText("Material Changes", "%d", m_materialChanges); // TODO get material changes from renderer
				ImGui::LabelText("Mesh Changes", "%d", m_meshChanges); // TODO get mesh changes from renderer
			}

			ImGui::End(); // end Render Settings Window
		}
	}
} // namespace Frac