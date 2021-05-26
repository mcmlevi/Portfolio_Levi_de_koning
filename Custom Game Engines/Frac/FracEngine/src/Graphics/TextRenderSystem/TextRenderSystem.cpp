#include "pch.h"

#include "Graphics/TextRenderSystem/TextRenderSystem.h"

#include "CoreRenderAPI/Components/Transform.h"
#include "Graphics/TextRenderSystem/TextMesh.h"

#include "Core/ECSWrapper.h"
#include "Core/EngineCore.h"

#include "Graphics/OpenGL.h"

namespace Frac 
{
	const std::string c_TextVertShader{ R"(#version 330 core
		layout(location = 0) in vec3 pos;
		layout(location = 1) in vec2 tex;
		out vec2 TexCoords;
		uniform mat4 projection;
		uniform mat4 model;
		void main()
		{
			gl_Position = projection * model * vec4(pos,1.0);
			TexCoords = tex;
		}  )" };

	const std::string c_TextFragShader{ R"(
		#version 330 core
		in vec2 TexCoords;
		out vec4 color;

		uniform sampler2D text;
		uniform vec3 textColor = vec3(1.0,1.0,1.0);

		void main()
		{    
		    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
		    color = vec4(textColor, 1.0) * sampled;
		}  
		)" };
	
	TextRenderSystem::TextRenderSystem(Frac::EngineCore* a_engineCore, ECSWrapper* a_flecsWorld) : 
		  m_engineCore(a_engineCore)
		, m_flecsWorld(a_flecsWorld)
		, m_textShader(std::make_shared<Frac::TextShader>())
		, m_viewProjection(glm::mat4(0.f))
		, m_camEntsQuery(a_flecsWorld->makeQuery<TOR::Camera>())
	{
		m_textShader->LoadFromString(c_TextVertShader, c_TextFragShader);

		ConfigFontAtlases();
		
		std::function<void(EntityHandle, TOR::Transform&, Frac::TextComponent&)> updateTextComponents = 
			[this](EntityHandle entity, TOR::Transform& transform, Frac::TextComponent& text)
		{
			// facing active camera to a certain location might not be relevant here
			if (text.faceActiveCamera)
			{
				FaceActiveCamera(transform);
			}

			if (text.text.empty())
			{
				return;
			}
				
			/////////////////////////////// TODO CoreRenderAPI
			if(text.dirtyText != text.text || text.mesh.m_verts.size() == 0)
			{
				SetTextMeshToEntityHandle(text);
			}
				
			m_textShader->Use();
				
			m_textShader->setVec3("textColor", text.color);
			m_textShader->setMat4("model", transform.ModelTransformData);
			m_textShader->setMat4("projection", m_viewProjection);
			
			DrawTextMesh(text.mesh);
			
			m_textShader->Unuse();
		};
		
		m_flecsWorld->makeSystem<TOR::Transform, Frac::TextComponent>(
			updateTextComponents, 
			ePipelineStage::postUpdate, 
			"Update Text Components");
	}

	void TextRenderSystem::SetTextMeshToEntityHandle(Frac::TextComponent& a_textComp)
	{
		a_textComp.dirtyText = a_textComp.text;
		
		auto glyphs = m_fontAtlases[a_textComp.fontFamily]->GetGlyphs(a_textComp.text);
		const auto amountOfGlyphs = glyphs.size();
		std::vector<Frac::Vertex> vertices;

		float avgHeight = 0.0f;
		float width = 0.0f;
		for (size_t i = 0; i < amountOfGlyphs; ++i)
		{
			float glyphH = (-glyphs[i].Quad.y0);
			avgHeight += (glyphH - avgHeight) / (i + 1);
			width += glyphs[i].Advance + glyphs[i].Kerning;
		}

		float begin = std::numeric_limits<float>::max();
		switch (a_textComp.alignment)
		{
		case TextComponent::Alignment::Left:
			begin = 0.0f;
			break;
		case TextComponent::Alignment::Center:
			begin = (width + tan(a_textComp.angle) * avgHeight) * -0.5f;
			break;
		case TextComponent::Alignment::Right:
			begin = -(width + tan(a_textComp.angle) * avgHeight);
			break;
		}

		// map glyphs to vertices 
		for (size_t i = 0; i < amountOfGlyphs; ++i)
		{
			const float glyphW = (glyphs[i].Quad.x1 - glyphs[i].Quad.x0);
			const float glyphH = (glyphs[i].Quad.y1 - glyphs[i].Quad.y0);
			unsigned short base = static_cast<unsigned short>(vertices.size());
			float slant = tan(a_textComp.angle) * glyphH;
			float underslant = tan(a_textComp.angle) * (glyphs[i].Quad.y1);

			// A sign, not a letter (private use section 0f unicode)
			if (glyphs[i].Codepoint > 0xE000 && glyphs[i].Codepoint < 0xF8FF)
			{
				slant = 0.0f;
				underslant = 0.0f;
			}

			// Create vertices
			Vertex verts[4] =
			{
				// Bottom
				{ glm::vec3(begin + glyphs[i].BearingX - underslant,(avgHeight / 2) + glyphs[i].Quad.y1,0.f),	 glm::vec2(glyphs[i].Quad.s0, glyphs[i].Quad.t1) }, //bottom left
				{ glm::vec3(begin + glyphs[i].BearingX + glyphW - underslant,(avgHeight / 2) + glyphs[i].Quad.y1,0.f), glm::vec2(glyphs[i].Quad.s1, glyphs[i].Quad.t1) }, //bottom right
				{ glm::vec3(begin + glyphs[i].BearingX + glyphW + slant - underslant,(avgHeight / 2) + glyphs[i].Quad.y0,0.f), glm::vec2(glyphs[i].Quad.s1, glyphs[i].Quad.t0) }, //top right
				{ glm::vec3(begin + glyphs[i].BearingX + slant - underslant,(avgHeight / 2) + glyphs[i].Quad.y0,0.f), glm::vec2(glyphs[i].Quad.s0, glyphs[i].Quad.t0) }  //top left 
			};

			const glm::vec3 mutliplier = glm::vec3(-1.f, -1.f, 1.f);
			verts[0].Position *= mutliplier;
			verts[1].Position *= mutliplier;
			verts[2].Position *= mutliplier;
			verts[3].Position *= mutliplier;
			
			begin += ((glyphs[i].Advance + glyphs[i].Kerning) + a_textComp.customKerning * tan(a_textComp.angle));

			//store vertices
			for (auto& v : verts)
			{
				v.Position *= a_textComp.fontSize;
				vertices.push_back(v);
			}

		} // end for glyphs
		
		a_textComp.mesh.m_verts = vertices;
		InitialiseTextMesh(a_textComp);
		/*
		TOR::EVertexAttributes attr = TOR::EVertexAttributes::POSITION;
		
		a_textComp.mesh = std::make_unique<TOR::OpenGLMesh>();
		a_textComp.mesh->Initialize();
		a_textComp.mesh->BufferAttributeData(attr, 
			vertices.size() * sizeof(Vertex), sizeof(Vertex), 3, &vertices[0]);
		
		attr = TOR::EVertexAttributes::TEXCOORD_0;
		
		a_textComp.mesh->BufferAttributeData(attr, 
			vertices.size() * sizeof(Vertex), sizeof(Vertex), 2, &vertices[0]);
		
		a_textComp.mesh->SetShader(m_textShader);
		*/
	}

	void TextRenderSystem::SetViewProjection(glm::mat4 a_vp)
	{
		m_viewProjection = a_vp;
	}

	void TextRenderSystem::InitialiseTextMesh(Frac::TextComponent& text)
	{
		Frac::TextMesh& mesh = text.mesh;
		
		if (m_fontAtlases[text.fontFamily])
			mesh.m_texture = static_cast<TextTexture>(*m_fontAtlases[text.fontFamily]);
		else
		{
			mesh.m_texture = TextTexture{};
			mesh.m_useTexture = false;
		}

		glGenVertexArrays(1, &mesh.m_vao);
        glGenBuffers(1, &mesh.m_vbo);
        glBindVertexArray(mesh.m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vbo);
    	
        glBufferData(GL_ARRAY_BUFFER, mesh.m_verts.size() * sizeof(Vertex), &mesh.m_verts[0], GL_STATIC_DRAW);
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex texture coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Texture)));

        glBindVertexArray(0);
	}

	void TextRenderSystem::DrawTextMesh(Frac::TextMesh& mesh)
	{
		// for alpha
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		
		glClear(GL_DEPTH_BUFFER_BIT); // remove depth information, makes sure this is rendered on top for now
		
		// draw mesh
		glBindVertexArray(mesh.m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vbo);
		if(mesh.m_useTexture)
		{
			glActiveTexture(GL_TEXTURE0); // activate proper texture unit before binding
			glBindTexture(GL_TEXTURE_2D, mesh.m_texture.GetID());

			glActiveTexture(GL_TEXTURE0);
		}

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));

		// vertex texture coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Texture)));

		for (unsigned int i = 0; i < mesh.m_verts.size(); i+=4)
		{
			unsigned int indices[4]{ i + 3, i + 2, i + 1, i };
			glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, &indices[0]);
		}
		glBindVertexArray(0);
		
		glEnable(GL_CULL_FACE);
	}

	void TextRenderSystem::ConfigFontAtlases()
	{
		m_fontAtlases[Frac::TextComponent::FontFamily::Regular]	= std::make_unique<Frac::FontAtlas>("RegularFont", 4096, 4096);
		m_fontAtlases[Frac::TextComponent::FontFamily::Icons] = std::make_unique<Frac::FontAtlas>("IconFont", 4096, 4096);

		m_fontAtlases[Frac::TextComponent::FontFamily::Regular]
			->AddRange(FileIO::GetPathFromWildcard("[Fonts]NotoMono-Regular.ttf"), 64.f, 0, 255);
		m_fontAtlases[Frac::TextComponent::FontFamily::Icons]
			->AddRange(FileIO::GetPathFromWildcard("[Fonts]heydings_controls.ttf"), 64.f, 0, 255);

		m_fontAtlases[Frac::TextComponent::FontFamily::Regular]->Apply();
		m_fontAtlases[Frac::TextComponent::FontFamily::Icons]->Apply();
	}

	Frac::EntityHandle TextRenderSystem::GetActiveCamera()
	{
		Frac::EntityHandle activeCam = 0;
		std::function<void(Frac::It, uint64_t, TOR::Camera*)> func = [&activeCam](Frac::It it, uint64_t index, TOR::Camera* camera) mutable
		{
			if(camera[index].IsActive)
			{
				activeCam = it.m_it.entity(index).id();
			}
		};
		m_camEntsQuery.Iter(func);

		return activeCam;
	}

	void TextRenderSystem::FaceActiveCamera(TOR::Transform& transform)
	{
		const Frac::EntityHandle camEnt = GetActiveCamera();
		if ( camEnt )
		{
			const TOR::Transform* camTrans = m_flecsWorld->get<TOR::Transform>(camEnt);

			transform.Orientation = SafeQuatLookAt(
				transform.ModelTransformData[3], // pos calculated based on potential parents
				camTrans->Position, 
				glm::vec3(0, 1, 0), 
				glm::vec3(0, -1, 0));
		}
	}

	glm::quat TextRenderSystem::SafeQuatLookAt(glm::vec3 const& lookFrom, glm::vec3 const& lookTo, glm::vec3 const& up,
	                                           glm::vec3 const& alternativeUp)
	{
		glm::vec3  direction       = lookTo - lookFrom;
		float      directionLength = glm::length(direction);

		// Check if the direction is valid; Also deals with NaN
		if(!(directionLength > 0.0001))
			return glm::quat(1, 0, 0, 0); // Just return identity

		// Normalize direction
		direction /= directionLength;

		// Is the normal up (nearly) parallel to direction?
		if(glm::abs(glm::dot(direction, up)) > .9999f) {
			// Use alternative up
			return glm::quatLookAt(direction, alternativeUp);
		}
		else {
			return glm::quatLookAt(direction, up);
		}
	}
} // namespace Frac
