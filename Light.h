#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"

namespace IF
{
	class LightManager
	{
		template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;
	
	private:
		static const int PLightNum = 3;
		static const int DLightNum = 3;
		static const int SLightNum = 3;
		static const int CShadowNum = 3;

	public:
		struct ConstBufferData
		{
			XMFLOAT3 ambientColor;
			float pad1;
			DLight::ConstDLightData dLights[DLightNum];
			PLight::ConstPLightData pLights[PLightNum];
			SLight::ConstSLightData sLights[PLightNum];
			CShadow::ConstCShadowData cShadows[CShadowNum];
		};

	private:
		static ID3D12Device* device;
		ComPtr<ID3D12Resource> constBuff;
		XMFLOAT3 ambientColor;
		DLight dLight[DLightNum];
		PLight pLight[PLightNum];
		SLight sLight[SLightNum];
		CShadow cShadow[CShadowNum];
		bool dirty = false;

	public:
		~LightManager();
		static void SetDevice(ID3D12Device* device);
		void Initialize();
		void TransferConstBuffer();
		void DefaultLightSetting();
		void SetAmbientColor(const XMFLOAT3& color);
		void SetDirLightActive(int index, bool active);
		void SetDirLightDir(int index, const XMVECTOR& lightdir);
		void SetDirLightColor(int index, const XMFLOAT3& lightcolor);
		void SetPointLightActive(int index, bool active);
		void SetPointLightPos(int index, const XMFLOAT3& lightpos);
		void SetPointLightColor(int index, const XMFLOAT3& lightcolor);
		void SetPointLightAtten(int index, const XMFLOAT3& lightAtten);
		void SetSpotLightActive(int index, bool active);
		void SetSpotLightDir(int index, const XMVECTOR& lightdir);
		void SetSpotLightPos(int index, const XMFLOAT3& lightpos);
		void SetSpotLightColor(int index, const XMFLOAT3& lightcolor);
		void SetSpotLightAtten(int index, const XMFLOAT3& lightAtten);
		void SetSpotLightFactorAngle(int index, const XMFLOAT2& lightFactorAngle);
		void SetCircleShadowActive(int index, bool active);
		void SetCircleShadowDir(int index, const XMVECTOR& shadowdir);
		void SetCircleShadowCasterPos(int index, const XMFLOAT3& shadowpos);
		void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
		void SetCircleShadowAtten(int index, const XMFLOAT3& shadowAtten);
		void SetCircleShadowFactorAngle(int index, const XMFLOAT2& shadowFactorAngle);
		void Update();
		void Draw(ID3D12GraphicsCommandList*commandList,UINT rootParameterIndex);
		static LightManager* GetInstance();
	};
}