#include "Scene2.h"
#include "Input.h"

using namespace IF;
using namespace DirectX;

IF::Scene2::Scene2(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
	:winWidth(winWidth), winHeight(winHeight), device(device), commandList(commandList), viewport(viewport)
{
	Graphic::SetDevice(device);
	Texture::setDevice(device);
	Model::SetDevice(device);
}

IF::Scene2::~Scene2()
{
	delete matPro;
	sound->Reset();
	Graphic::DeleteInstance();
}

void IF::Scene2::Initialize()
{
	light = LightManager::Instance();
	light->Initialize();
	light->DefaultLightSetting();
	for (int i = 0; i < 3; i++)
	{
		light->SetDirLightActive(i, true);
		light->SetPointLightActive(i, false);
		light->SetSpotLightActive(i, false);
	}
	light->SetCircleShadowActive(0, true);
	light->SetAmbientColor({ 1, 1, 1 });
	Object::SetLight(light);
	//定数バッファの初期化
	cb.Initialize(device.Get());

	//画像関連初期化
	graph->Initialize(tex->descRangeSRV, L"Resources/Shaders/ModelVS.hlsl", L"Resources/Shaders/ModelPS.hlsl", L"Resources/Shaders/ModelGS.hlsl");
	graph->Initialize2D(tex->descRangeSRV, L"Resources/Shaders/SpriteVS.hlsl", L"Resources/Shaders/SpritePS.hlsl");

	//モデルの読み込みとオブジェクトとの紐付け(空と地面)
	tex->Initialize();
	texNum = tex->LoadTexture("Resources/mario.jpg");
	cube.CreateCube();
	obj.Initialize(device.Get(), &cube);
	obj.position = { 0, 1, 0 };
	obj.scale = { 5, 5, 5 };
	obj.rotation = { 0,0,0 };

	//カメラ関連初期化
	matPro = new Projection(45.0f, (float)winWidth, (float)winHeight);

	//そのほかの初期化
	matView.eye = { 0,0,-300 };
	matView.target = { 0,1,0 };
	matView.Update();


	//2D関連
	sprite.StaticInitialize(device.Get(), commandList.Get(), (float)winWidth, (float)winHeight);
	sprite.Initialize(0);
	dText.Initialize(tex->LoadTexture("Resources/debugfont.png"));
}

void IF::Scene2::Update()
{
	Input* input = Input::Instance();
	input->Update();

	//光源
	static XMFLOAT3 dlColor = { 1,1,1 };

	//if (input->KDown(KEY::W))lightDir.m128_f32[1] += 1.0f;
	//if (input->KDown(KEY::S))lightDir.m128_f32[1] -= 1.0f;
	//if (input->KDown(KEY::D))lightDir.m128_f32[0] += 1.0f;
	//if (input->KDown(KEY::A))lightDir.m128_f32[0] -= 1.0f;

	light->SetCircleShadowDir(0, { csDir.x,csDir.y,csDir.z,0 });
	light->SetCircleShadowAtten(0, csAtten);
	light->SetCircleShadowFactorAngle(0, csAngle);

	for (int i = 0; i < 3; i++)light->SetDirLightColor(i, dlColor);

	//カメラ
	//if (input->KDown(KEY::UP))
	//{
	//	matView.eye.z += 0.5f;
	//	matView.target.z += 0.5f;
	//}
	//if (input->KDown(KEY::DOWN))
	//{
	//	matView.eye.z -= 0.5f;
	//	matView.target.z -= 0.5f;
	//}
	//if (input->KDown(KEY::RIGHT))
	//{
	//	matView.eye.x += 0.5f;
	//	matView.target.x += 0.5f;
	//}
	//if (input->KDown(KEY::LEFT))
	//{
	//	matView.eye.x -= 0.5f;
	//	matView.target.x -= 0.5f;
	//}

	static int rota = 0;
	static float posx = 0;
	static float posz = 0;
	rota++;
	posx = cosf(ConvertToRadians(rota)) * 300;
	posz = sinf(ConvertToRadians(rota)) * 300;

	matView.eye = { posx,1,posz };

	matView.Update();
	light->Update();

	obj.Update(matView.Get(), matPro->Get(), matView.eye);

	sprite.position = { 540,500 };
	sprite.Update();
}

void IF::Scene2::Draw()
{
	graph->DrawBlendMode(commandList.Get());
	obj.DrawBefore(commandList.Get(), graph->rootsignature.Get(), cb.GetGPUAddress());
	obj.Draw(commandList.Get(), viewport, texNum);
}
