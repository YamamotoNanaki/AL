#include "Scene2.h"
#include "Input.h"
#include "Ease.h"
#include "Timer.h"

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
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Initialize(device.Get(), &cube);
		obj[i].scale = { 3, 3, 3 };
		obj[i].rotation = { 0,0,0 };
	}
	obj[0].position = { 0, 50, 0 };
	obj[1].position = { 50, -50, 0 };
	obj[2].position = { -50, -50, 0 };

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
	static short flagNum = 0;
	static bool flag = false;
	static Float3 target{};
	static Float3 start{};
	static Timer timer;

	if (input->KTriggere(KEY::SPACE) && !flag)
	{
		start = Float3Convert(matView.target);
		flag = true;
		flagNum++;
		timer.Set(30);
		if (flagNum == 3)flagNum = 0;
		if (flagNum == 0)target = Float3Convert(obj[0].position);
		if (flagNum == 1)target = Float3Convert(obj[1].position);
		if (flagNum == 2)target = Float3Convert(obj[2].position);
	}

	if (flag)
	{
		timer.Update();
		matView.target.x = Ease::InOutQuad(start.x, target.x, 30, timer.NowTime());
		matView.target.y = Ease::InOutQuad(start.y, target.y, 30, timer.NowTime());
		matView.target.z = Ease::InOutQuad(start.z, target.z, 30, timer.NowTime());
		if (timer.IsEnd())flag = false;
	}

	matView.Update();
	light->Update();
	for (int i = 0; i < _countof(obj); i++)
		obj[i].Update(matView.Get(), matPro->Get(), matView.eye);

	sprite.position = { 540,500 };
	sprite.Update();

	dText.Print(50, 50, 1.5, "eye    : %f,%f,%f", matView.eye.x, matView.eye.y, matView.eye.z);
	dText.Print(50, 75, 1.5, "target : %f,%f,%f", matView.target.x, matView.target.y, matView.target.z);
	dText.Print(50, 100, 1.5, "up     : %f,%f,%f", matView.up.x, matView.up.y, matView.up.z);
}

void IF::Scene2::Draw()
{
	graph->DrawBlendMode(commandList.Get());
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].DrawBefore(commandList.Get(), graph->rootsignature.Get(), cb.GetGPUAddress());
		obj[i].Draw(commandList.Get(), viewport, texNum);
	}
	graph->DrawBlendMode(commandList.Get(), Blend::NORMAL2D);
	sprite.DrawBefore(graph->rootsignature.Get(), cb.GetGPUAddress());
	dText.Draw(viewport);
}
