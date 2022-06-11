#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>
#include "Light.h"
#include "ModelVI.h"
#include "ConstStruct.h"
#include "Model.h"
#include "MathConvert.h"

#pragma comment(lib,"d3d12.lib") 

namespace IF
{
	namespace BillBoard
	{
		enum BillBoardMode
		{
			NOON,
			BILLBOARD,
			YBOARD
		};
	}

	class Object
	{
		template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
		template<class T> using vector = std::vector<T>;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMMATRIX = DirectX::XMMATRIX;
	private:
		Model* model = nullptr;
		static LightManager* light;

	public:
		//定数バッファ
		ComPtr<ID3D12Resource> constBuffTransform;
		//定数バッファマップ
		ConstBufferDataTransform* constMapTransform = nullptr;
		//アフィン変換情報
		XMFLOAT3 scale = { 1,1,1 };
		XMFLOAT3 rotation = { 0,0,0 };
		XMFLOAT3 position = { 0,0,0 };
		//ワールド変換行列
		XMMATRIX matWorld{};
		//親オブジェクトへのポインタ
		Object* parent = nullptr;

	public:
		void Initialize(ID3D12Device* device, Model* model);
		void SetModel(Model* model);
		void DrawBefore(ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* root, D3D12_GPU_VIRTUAL_ADDRESS GPUAddress,
			D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Update(XMMATRIX matView, XMMATRIX matProjection, XMFLOAT3 comeraPos, BillBoard::BillBoardMode mode = BillBoard::NOON);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport);
		void Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport, unsigned short texNum);
		~Object();
		static void SetLight(LightManager* light)
		{
			Object::light = light;
		}
	};
}