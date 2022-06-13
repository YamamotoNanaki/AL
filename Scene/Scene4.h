#pragma once
#include "IScene.h"
#include "ConstBuff.h"
#include "Object.h"
#include "Model.h"
#include "View.h"
#include "Projection.h"
#include "Texture.h"
#include "Graphic.h"
#include "Fire.h"
#include "Light.h"
#include "Sprite.h"
#include "Sound.h"
#include "DebugText.h"

namespace IF
{
	class Scene4 : public IScene
	{

		template <class T>using vector = std::vector<T>;
		template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	private:
		//ïœêî----------------------//
		Object obj[100];
		Model cube;
		DirectX::XMFLOAT3 csDir{ 0.0f,-1.0f,0.0f };
		DirectX::XMFLOAT3 csAtten{ 0.5f,0.6f,0.0f };
		DirectX::XMFLOAT2 csAngle{ 0.0f,0.7f };
		unsigned short texNum = 0;

		//--------------------------//

		//ÉJÉÅÉâ
		View matView;
		Projection* matPro = nullptr;
		//åıåπ
		LightManager* light = nullptr;
		//âπåπ
		Sound* sound = Sound::Instance();
		//2d
		Sprite sprite{};
		DebugText dText;

	public:
		ConstBuff cb;
		Texture* tex = Texture::Instance();
		Graphic* graph = Graphic::Instance();

	private:
		int winWidth;
		int winHeight;
		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		vector<D3D12_VIEWPORT> viewport;

	public:
		Scene4(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport);
		~Scene4();

		void Initialize();
		void Update();
		void Draw();
	};
}
