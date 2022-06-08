#pragma once
#include "Object.h"

class Player
{
private:
	IF::Object obj[5];
	DirectX::XMFLOAT3 front = { 0,0,0 };

public:
	void Update(DirectX::XMMATRIX matView, DirectX::XMMATRIX matProjection, DirectX::XMFLOAT3 cameraPos, DirectX::XMFLOAT3 cameraTarget);
	void Draw(ID3D12GraphicsCommandList* commandList, std::vector<D3D12_VIEWPORT> viewport);
	void Initialize(IF::Model* model, DirectX::XMMATRIX matView, DirectX::XMMATRIX matProjection, DirectX::XMFLOAT3 cameraPos, ID3D12Device* device);
	DirectX::XMFLOAT3 GetFront()const;
	DirectX::XMFLOAT3 GetPos()const;
};