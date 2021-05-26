#include <Thpch.h>
#include <Resources/D3D12/D3D12Vertex.h>

namespace Th
{
	const D3D12_INPUT_ELEMENT_DESC D3D12Vertex::InputElements[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	bool D3D12Vertex::operator==(const D3D12Vertex& rhs) const
	{
		return (Pos == rhs.Pos) && (Color == rhs.Color);
	}
}