#pragma once
#include "IO/ModelBuilder.h"

namespace Frac
{
	/// <summary>
	/// The model director is responsable for deciding in what order models should be loaded in.
	/// </summary>
	class ModelDirector final
	{
	private:
		ModelBuilder* m_Builder; //#TODO: Template this, so C pointers can be avoided.

	public:
		ModelDirector() = default;
		~ModelDirector() = default;

		void SetBuilder(ModelBuilder* modelBuilder);
		void Construct();
	};
}