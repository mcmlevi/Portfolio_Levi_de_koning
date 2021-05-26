#pragma once
#include <glm/glm.hpp>

namespace Th
{
	/// <summary>
	/// The vertex representation of the directX 12 shader.
	/// </summary>
	struct D3D12Vertex
	{
		glm::vec3 Pos;
		glm::vec3 Color;

		bool operator==(const D3D12Vertex& rhs) const;

		static const int InputElementCount = 2;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}