#pragma once
#include "Tools/PC/Window/IGUIWindow.h"

namespace Frac
{
	class RenderSettings : public IGUIWindow
	{
	public:
		RenderSettings();
		~RenderSettings();

		void Update() override;

	private:
		// Temporary member variables untill we can get this data from the renderer
		unsigned int m_attenuation;
		int m_debugPass;
		bool m_blit;
		bool m_hdr;
		unsigned int m_toneMapping;
		float m_exposure;
		float m_maxWhite;
		int m_materialChanges;
		int m_meshChanges;
	};
} // namespace Frac