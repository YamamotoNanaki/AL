#pragma once
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <unordered_map>

namespace IF
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;			//XYZ座標
		DirectX::XMFLOAT3 normal;		//法線ベクトル
		DirectX::XMFLOAT2 uv;			//UV座標
	};
	enum NormalFlag
	{
		NFALSE = false,
		NTRUE = true
	};
	class MVI
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff = nullptr;
		std::vector<Vertex> vertices;
		std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;

	public:
		std::vector<unsigned short> indices;
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		D3D12_INDEX_BUFFER_VIEW ibView{};

	public:
		void SetVerticleIndex(std::vector<Vertex> vertices, size_t vertexCount, std::vector<unsigned short> indices, size_t indexCount);
		void Initialize(ID3D12Device* device, bool smoothing, NormalFlag flag = NFALSE);
		D3D12_VERTEX_BUFFER_VIEW& GetVertexView();
		D3D12_INDEX_BUFFER_VIEW& GetIndexView();
		size_t GetSize();
		void AddSmoothData(unsigned short indexPos, unsigned short indexVer);
	};
}