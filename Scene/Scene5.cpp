#include "Scene5.h"
#include "Input.h"
#include "Rand.h"

using namespace IF;
using namespace DirectX;

IF::Scene5::Scene5(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
	:winWidth(winWidth), winHeight(winHeight), device(device), commandList(commandList), viewport(viewport)
{
	Graphic::SetDevice(device);
	Texture::setDevice(device);
	Model::SetDevice(device);
}

IF::Scene5::~Scene5()
{
	delete matPro;
	sound->Reset();
	Graphic::DeleteInstance();
}

void IF::Scene5::Initialize()
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

	//カメラ関連初期化
	matPro = new Projection(45.0f, (float)winWidth, (float)winHeight);

	//そのほかの初期化
	matView.eye = { 0,0,-100 };
	matView.Update();


	//2D関連
	sprite.StaticInitialize(device.Get(), commandList.Get(), (float)winWidth, (float)winHeight);
	sprite.Initialize(0);
	dText.Initialize(tex->LoadTexture("Resources/debugfont.png"));

	Rand random;
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Initialize(device.Get(), &cube);
		obj[i].scale = { 1, 1, 1 };
	}
	obj[0].position = { 0,10,0 };

	obj[1].parent = &obj[7];
	obj[1].position = { 0,15,0 };
	obj[2].parent = &obj[7];
	obj[2].position = { 15,0,0 };
	obj[3].parent = &obj[7];
	obj[3].position = { -15,0,0 };
	obj[4].parent = &obj[0];
	obj[4].position = { 0,-15,0 };
	obj[5].parent = &obj[4];
	obj[5].position = { 15,-15,0 };
	obj[6].parent = &obj[4];
	obj[6].position = { -15,-15,0 };
	obj[7].parent = &obj[0];
	obj[7].position = { 0,0,0 };
	obj[8].parent = &obj[5];
	obj[8].position = { 0,-10,0 };
	obj[9].parent = &obj[6];
	obj[9].position = { 0,-10,0 };
	obj[10].parent = &obj[2];
	obj[10].position = { 0,-10,0 };
	obj[11].parent = &obj[3];
	obj[11].position = { 0,-10,0 };
}

void IF::Scene5::Update()
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

	//if (input->KDown(KEY::UP))matPro->nearZ++;
	//if (input->KDown(KEY::DOWN))matPro->nearZ--;
	//if (input->KDown(KEY::RIGHT))matPro->farZ++;
	//if (input->KDown(KEY::LEFT))matPro->farZ--;

	matPro->Update();

	static float rota2 = 0;

	//if (input->KDown(KEY::W))obj[0].position.y += 2;
	//if (input->KDown(KEY::S))obj[0].position.y -= 2;
	if (input->KDown(KEY::RIGHT))obj[0].position.x += 2;
	if (input->KDown(KEY::LEFT))obj[0].position.x -= 2;
	if (input->KDown(KEY::D))rota2 += 4;
	if (input->KDown(KEY::A))rota2 -= 4;

	obj[0].rotation.y = ConvertToRadians(rota2);

	static int rota = 0;
	static bool flag = false;
	bool flag2 = false;
	if (input->KDown(KEY::W))flag2 = true;
	if (flag2)
	{
		if (flag == false)rota += 6;
		if (flag == true)rota -= 6;
		if (rota >= 45)flag = true;
		if (rota <= -45)flag = false;
	}
	else
	{
		if (rota == 0);
		else if (rota > 0)rota -= 6;
		else if (rota < 0)rota += 6;
	}
	if (rota2 >= 360)rota2 -= 360;
	static bool flag3 = false;
	static float powor = 0;

	if (input->KTriggere(KEY::SPACE))
	{
		flag3 = true;
		powor = 7;
	}
	if (flag3)
	{
		obj[0].position.y += powor;
		powor--;
		if (obj[0].position.y < 10)
		{
			flag3 = false;
			obj[0].position.y = 10;
		}
	}
	//if (input->KDown(KEY::U))rota -= 2;
	//if (input->KDown(KEY::I))rota += 2;
	//static float rota2 = 0;
	//if (input->KDown(KEY::J))rota2 -= 2;
	//if (input->KDown(KEY::K))rota2 += 2;
	float rotaf = rota;
	obj[2].rotation = { ConvertToRadians(rotaf),0,0 };
	obj[3].rotation = { ConvertToRadians(-rotaf),0,0 };
	obj[5].rotation = { ConvertToRadians(-rotaf), 0,0 };
	obj[6].rotation = { ConvertToRadians(rotaf), 0,0 };

	//カメラ
	//if (input->KDown(KEY::UP))
	//{
	//	matView.eye.z += 2;
	//	//matView.target.z += 0.5f;
	//}
	//if (input->KDown(KEY::DOWN))
	//{
	//	matView.eye.z -= 2;
	//	//matView.target.z -= 0.5f;
	//}
	//if (input->KDown(KEY::RIGHT))
	//{
	//	//matView.eye.x += 0.5f;
	//	matView.target.x += 2;
	//}
	//if (input->KDown(KEY::LEFT))
	//{
	//	//matView.eye.x -= 0.5f;
	//	matView.target.x -= 2;
	//}
	//if (input->KDown(KEY::SPACE))rota++;
	//if (rota >= 360)rota = 0;
	//matView.up = { cosf(ConvertToRadians(rota)),sinf(ConvertToRadians(rota)),0 };

	matView.Update();
	light->Update();
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Update(matView.Get(), matPro->Get(), matView.eye);
	}
	sprite.position = { 540,500 };
	sprite.Update();

}

void IF::Scene5::Draw()
{
	graph->DrawBlendMode(commandList.Get());
	obj[0].DrawBefore(commandList.Get(), graph->rootsignature.Get(), cb.GetGPUAddress());
	for (int i = 1; i < _countof(obj); i++)
	{
		obj[i].Draw(commandList.Get(), viewport, texNum);
	}
	graph->DrawBlendMode(commandList.Get(), Blend::NORMAL2D);
	sprite.DrawBefore(graph->rootsignature.Get(), cb.GetGPUAddress());
	dText.Draw(viewport);
}
