#pragma once
#include <Resources/D3D12/D3D12VertexBuffer.h>
#include <Resources/D3D12/D3D12IndexBuffer.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Resources/D3D12/D3D12Vertex.h>

namespace Th
{
	class D3D12Device;
	class D3D12CommandList;

	/// <summary>
	/// Primitive types used to create multiple debug primitives.
	/// </summary>
	enum class EPrimitiveType
	{
		Cube = 0,
		Sphere,
		Cone,
		Torus,
		Plane,

		COUNT
	};

	/// <summary>
	/// A DirectX 12 mesh containing a vertex and index buffer.
	/// </summary>
	class D3D12Mesh
	{
	public:
		D3D12Mesh() = default;
		virtual ~D3D12Mesh() = default;

		void Initialize(D3D12Device* device, D3D12CommandList* commandList, const std::vector<D3D12Vertex>& vertices, const std::vector<uint16_t>& indices);

		static D3D12Mesh* CreateCube(D3D12Device* device, D3D12CommandList* commandList, float size);
		//static D3D12Mesh* CreateSphere(D3D12Device* device, D3D12CommandList* commandList, float diameter = 1, size_t tessellation = 16);
		//static D3D12Mesh* CreateCone(D3D12Device* device, D3D12CommandList* commandList, float diameter = 1, float height = 1, size_t tesselation = 32);
		//static D3D12Mesh* CreateTorus(D3D12Device* device, D3D12CommandList* commandList, float diameter = 1, float thickness = 0.333f, size_t tesselation = 32);
		//static D3D12Mesh* CreatePlane(D3D12Device* device, D3D12CommandList* commandList, float width = 1, float height = 1);

		size_t GetIndexCount() const;

		void RecordDrawCommand(D3D12CommandList* commandList, uint32_t instanceCount, uint32_t firstInstance);

	private:
		D3D12VertexBuffer m_VertexBuffer;
		D3D12IndexBuffer m_IndexBuffer;
	};
}