#pragma once
#include <DirectXMath.h>

namespace IF
{
	class PLight
	{
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		struct ConstPLightData
		{
			XMFLOAT3 lightPos;
			float pad1;
			XMFLOAT3 lightColor;
			float pad2;
			XMFLOAT3 lightAtten;
			unsigned int active;
		};

	private:
		XMFLOAT3 lightPos = { 0,0,0 };
		XMFLOAT3 lightColor = { 1,1,1 };
		XMFLOAT3 lightAtten = { 1.0f,1.0f,1.0f };
		bool active = false;

	public:
		inline void SetLightPos(const XMFLOAT3& lightPos) { this->lightPos = lightPos; }
		inline const XMFLOAT3 GetLightPos() { return lightPos; }
		inline void SetLightColor(const XMFLOAT3& lightColor) { this->lightColor = lightColor; }
		inline const XMFLOAT3 GetLightColor() { return lightColor; }
		inline void SetLightAtten(const XMFLOAT3& lightAtten) { this->lightAtten = lightAtten; }
		inline const XMFLOAT3 GetLightAtten() { return lightAtten; }
		inline void SetActive(bool active) { this->active = active; }
		inline bool IsActive() { return active; }
	};
}