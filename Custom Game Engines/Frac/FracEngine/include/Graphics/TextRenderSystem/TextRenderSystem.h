#pragma once

#include "TextShader.h"
#include "Core/ECSWrapper.h"

#include "Graphics/TextRenderSystem/Font.h"
#include "Graphics/Components/TextComponent.h"

namespace Frac 
{
	class FileIO;
	class EngineCore;
	struct TextMesh;
	struct TextComponent;
	class ECSWrapper;

	class TextRenderSystem {
	public:
		TextRenderSystem(EngineCore* a_renderSystem, ECSWrapper* a_flecsWorld);
		~TextRenderSystem() = default;

		void SetTextMeshToEntityHandle(Frac::TextComponent& a_textComp);

		void SetViewProjection(glm::mat4 a_vp);
		
	private:
		void InitialiseTextMesh(Frac::TextComponent& text);
		void DrawTextMesh(Frac::TextMesh& mesh);

		void ConfigFontAtlases();

		Frac::EntityHandle GetActiveCamera();

		void FaceActiveCamera(TOR::Transform& transform);
		// https://stackoverflow.com/questions/18172388/glm-quaternion-lookat-function
		glm::quat SafeQuatLookAt( 
			glm::vec3 const& lookFrom,
			glm::vec3 const& lookTo,
			glm::vec3 const& up,
			glm::vec3 const& alternativeUp);
		
		EngineCore* m_engineCore;
		ECSWrapper* m_flecsWorld;

		std::shared_ptr<Frac::TextShader> m_textShader;
		std::unordered_map<Frac::TextComponent::FontFamily, std::unique_ptr<Frac::FontAtlas>> m_fontAtlases;
		glm::mat4 m_viewProjection;

		Frac::Query<TOR::Camera> m_camEntsQuery;
	};
} // namespace Frac