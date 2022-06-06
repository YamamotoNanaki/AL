#pragma once
#include <DirectXMath.h>

namespace IF
{
	class DLight
	{
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;
	public:
		struct ConstDLightData
		{
			XMVECTOR lightv;
			XMFLOAT3 lightColor;
			unsigned int active;
		};

	private:
		XMVECTOR lightDir = { 1,0,0,0 };
		XMFLOAT3 lightColor = { 1,1,1 };
		bool active = false;

	public:
		inline void SetLightDir(const XMVECTOR& lightDir) { this->lightDir = DirectX::XMVector3Normalize(lightDir); }
		inline const XMVECTOR& GetLightDir() { return lightDir; }
		inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor = lightColor; }
		inline const XMFLOAT3& GetLightColor() { return lightColor; }
		inline void SetActive(bool active) { this->active = active; }
		inline bool IsActive() { return active; }
	};
}