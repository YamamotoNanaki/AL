#include "Scene3.h"
#include "Input.h"
#include "Rand.h"
#include "Ease.h"
#include "Timer.h"

using namespace IF;
using namespace DirectX;

IF::Scene3::Scene3(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
	:winWidth(winWidth), winHeight(winHeight), device(device), commandList(commandList), viewport(viewport)
{
	Graphic::SetDevice(device);
	Texture::setDevice(device);
	Model::SetDevice(device);
}

IF::Scene3::~Scene3()
{
	delete matPro;
	sound->Reset();
	Graphic::DeleteInstance();
}

void IF::Scene3::Initialize()
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
	matView.eye = { 0,0,-300 };
	matView.Update();


	//2D関連
	sprite.StaticInitialize(device.Get(), commandList.Get(), (float)winWidth, (float)winHeight);
	sprite.Initialize(tex->LoadTexture("Resources/reticle.png"));
	sprite.position = { (float)winWidth / 2,(float)winHeight / 2 };
	sprite.Update();
	dText.Initialize(tex->LoadTexture("Resources/debugfont.png"));

	Rand random;
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Initialize(device.Get(), &cube);
		obj[i].position = { random.GetRandF(-80,80), random.GetRandF(-80,80), random.GetRandF(-80,80) };
		obj[i].scale = { 1, 1, 1 };
		obj[i].rotation = { random.GetRandF(0,360),random.GetRandF(0,360),0 };
	}
}

void IF::Scene3::Update()
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

	static float zoom = 45.0f;
	//カメラ
	if (input->KDown(KEY::UP))
	{
		//matView.eye.z += 2;
		matView.target.y += zoom > 40 ? 2 : zoom > 20 ? 1 : 0.5;
	}
	if (input->KDown(KEY::DOWN))
	{
		//matView.eye.z -= 2;
		matView.target.y -= zoom > 40 ? 2 : zoom > 20 ? 1 : 0.5;
	}
	if (input->KDown(KEY::LEFT))
	{
		//matView.eye.x += 2;
		matView.target.x -= zoom > 40 ? 2 : zoom > 20 ? 1 : 0.5;
	}
	if (input->KDown(KEY::RIGHT))
	{
		//matView.eye.x -= 2;
		matView.target.x += zoom > 40 ? 2 : zoom > 20 ? 1 : 0.5;
	}

	static float zoomT = 0;
	static float oldzoomT = 0;
	static bool flag2 = false;
	static const float max = 15;
	static Timer timer;
	if (input->KTriggere(KEY::SPACE))
	{
		zoom = zoom == 45 ? 30 : 45;
		flag = !flag;
	}
	if (flag)
	{
		if (input->KTriggere(KEY::S) && !flag2)
		{
			zoomT = 30.0f;
			oldzoomT = zoom;
			flag2 = true;
			timer.Set(max);
		}
		if (input->KTriggere(KEY::W) && !flag2)
		{
			zoomT = 10.0f;
			oldzoomT = zoom;
			flag2 = true;
			timer.Set(max);
		}
	}
	if (flag2)
	{
		timer.Update();
		zoom = Ease::InOutQuad(oldzoomT, zoomT, max, timer.NowTime());
		if (timer.IsEnd())flag2 = false;
	}
	matPro->fovAngle = zoom;
	//static int rota = 90;
	//if (input->KDown(KEY::SPACE))rota++;
	//if (rota >= 360)rota = 0;
	//matView.up = { cosf(ConvertToRadians(rota)),sinf(ConvertToRadians(rota)),0 };
	matPro->Update();
	matView.Update();
	light->Update();
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Update(matView.Get(), matPro->Get(), matView.eye);
	}

	dText.Print(50, 50, 1.5, "eye      : %f,%f,%f", matView.eye.x, matView.eye.y, matView.eye.z);
	dText.Print(50, 75, 1.5, "target   : %f,%f,%f", matView.target.x, matView.target.y, matView.target.z);
	dText.Print(50, 100, 1.5, "up      : %f,%f,%f", matView.up.x, matView.up.y, matView.up.z);
	dText.Print(50, 125, 1.5, "fovAngle: %f", zoom);
}

void IF::Scene3::Draw()
{
	graph->DrawBlendMode(commandList.Get());
	obj[0].DrawBefore(commandList.Get(), graph->rootsignature.Get(), cb.GetGPUAddress());
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Draw(commandList.Get(), viewport, texNum);
	}
	graph->DrawBlendMode(commandList.Get(), Blend::NORMAL2D);
	sprite.DrawBefore(graph->rootsignature.Get(), cb.GetGPUAddress());
	if (flag)sprite.Draw(viewport);
	dText.Draw(viewport);
}
