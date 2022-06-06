#pragma once
#include <DirectXMath.h>

namespace IF
{
	class CShadow
	{
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		struct ConstCShadowData
		{
			XMVECTOR shadowv;
			XMFLOAT3 casterPos;
			float distanceCasterLight;
			XMFLOAT3 shadowAtten;
			float pad3;
			XMFLOAT2 shadowFactorAngleCos;
			unsigned int active = false;
			float pad4;
		};

	private:
		XMVECTOR shadowDir = { 1,0,0,0 };
		float distanceCasterLight = 100.0f;
		XMFLOAT3 casterPos = { 0,0,0 };
		XMFLOAT3 shadowAtten = { 0.5f,0.6f,0.0f };
		XMFLOAT2 shadowFactorAngleCos = { 0.2f,0.5f };
		bool active = false;

	public:
		inline void SetShadowDir(const XMVECTOR& shadowDir) { this->shadowDir = DirectX::XMVector3Normalize(shadowDir); }
		inline const XMVECTOR& GetShadowDir() { return shadowDir; }
		inline void SetCasterPos(const XMFLOAT3& casterPos) { this->casterPos = casterPos; }
		inline const XMFLOAT3& GetCasterPos() { return casterPos; }
		inline void SetDistanceCasterLight(float distanceCasterLight) { this->distanceCasterLight = distanceCasterLight; }
		inline const float GetDistanceCasterLight() { return distanceCasterLight; }
		inline void SetShadowAtten(const XMFLOAT3& shadowAtten) { this->shadowAtten = shadowAtten; }
		inline const XMFLOAT3& GetShadowAtten() { return shadowAtten; }
		inline void SetShadowFactorAngle(const XMFLOAT2& shadowAngle)
		{
			this->shadowFactorAngleCos.x = cosf(DirectX::XMConvertToRadians(shadowAngle.x));
			this->shadowFactorAngleCos.y = cosf(DirectX::XMConvertToRadians(shadowAngle.y));
		}
		inline const XMFLOAT2& GetShadowFactorAngle() { return shadowFactorAngleCos; }
		inline void SetActive(bool active) { this->active = active; }
		inline bool IsActive() { return active; }
	};
}