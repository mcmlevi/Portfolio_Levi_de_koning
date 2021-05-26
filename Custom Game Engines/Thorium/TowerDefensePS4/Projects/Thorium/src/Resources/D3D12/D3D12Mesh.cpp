#include <Thpch.h>
#include <Graphics/D3D12/D3D12Device.h>
#include <Resources/D3D12/D3D12Mesh.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Resources/D3D12/D3D12Vertex.h>

namespace Th
{
	void D3D12Mesh::Initialize(D3D12Device* device, D3D12CommandList* commandList, const std::vector<D3D12Vertex>& vertices, const std::vector<uint16_t>& indicies)
	{
		assert(vertices.size() < USHORT_MAX && "Too many vertices for 16-bit index buffer!");

		m_VertexBuffer.Initialize(device);
		m_VertexBuffer.SetName(L"VertexBuffer");
		commandList->CopyVertexBuffer(m_VertexBuffer, vertices);

		m_IndexBuffer.Initialize(device);
		m_IndexBuffer.SetName(L"IndexBuffer");
		commandList->CopyIndexBuffer(m_IndexBuffer, indicies);
	}

	D3D12Mesh* D3D12Mesh::CreateCube(D3D12Device* device, D3D12CommandList* commandList, float size)
	{
		const int vertexCount = 8;

		std::vector<D3D12Vertex> vertices = {
			{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec3(0.0f, 0.0f, 0.0f) },
			{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec3(0.0f, 1.0f, 0.0f) },
			{ glm::vec3(1.0f,  1.0f, -1.0f),	glm::vec3(1.0f, 1.0f, 0.0f) },
			{ glm::vec3(1.0f, -1.0f, -1.0f),	glm::vec3(1.0f, 0.0f, 0.0f) },
			{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec3(0.0f, 0.0f, 1.0f) },
			{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec3(0.0f, 1.0f, 1.0f) },
			{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec3(1.0f, 1.0f, 1.0f) },
			{ glm::vec3(1.0f, -1.0f,  1.0f),	glm::vec3(1.0f, 0.0f, 1.0f) }
		};

		size /= 2;

		for (int i = 0; i < vertexCount; i++)
		{
			vertices[i].Pos *= size;
		}

		std::vector<uint16_t> indices =
		{
			0, 1, 2, 0, 2, 3,
			4, 6, 5, 4, 7, 6,
			4, 5, 1, 4, 1, 0,
			3, 2, 6, 3, 6, 7,
			1, 5, 6, 1, 6, 2,
			4, 0, 3, 4, 3, 7
		};

		D3D12Mesh* mesh = new D3D12Mesh();
		mesh->Initialize(device, commandList, vertices, indices);

		return mesh;
	}

	//Th::D3D12Mesh* D3D12Mesh::CreateSphere(D3D12Device* device, D3D12CommandList* commandList, float diameter /*= 1*/, size_t tessellation /*= 16*/)
	//{
	//
	//}
	//
	//Th::D3D12Mesh* D3D12Mesh::CreateCone(D3D12Device* device, D3D12CommandList* commandList, float diameter /*= 1*/, float height /*= 1*/, size_t tesselation /*= 32*/)
	//{
	//
	//}
	//
	//Th::D3D12Mesh* D3D12Mesh::CreateTorus(D3D12Device* device, D3D12CommandList* commandList, float diameter /*= 1*/, float thickness /*= 0.333f*/, size_t tesselation /*= 32*/)
	//{
	//
	//}
	//
	//Th::D3D12Mesh* D3D12Mesh::CreatePlane(D3D12Device* device, D3D12CommandList* commandList, float width /*= 1*/, float height /*= 1*/)
	//{
	//
	//}

	size_t D3D12Mesh::GetIndexCount() const
	{
		return m_IndexBuffer.GetNumIndicies();
	}

	void D3D12Mesh::RecordDrawCommand(D3D12CommandList* commandList, uint32_t instanceCount, uint32_t firstInstance)
	{
		commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->SetVertexBuffer(0, m_VertexBuffer);
		size_t indexCount = m_IndexBuffer.GetNumIndicies();

		if (indexCount > 0)
		{
			commandList->SetIndexBuffer(m_IndexBuffer);
			commandList->DrawIndexed(indexCount, instanceCount, 0, 0, firstInstance);
		}
		else
		{
			commandList->Draw(m_VertexBuffer.GetVertexCount(), instanceCount, 0, firstInstance);
		}
	}

}