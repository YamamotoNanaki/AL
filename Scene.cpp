#include "Scene.h"
#include "Input.h"
#include <DirectXMath.h>
#include "Rand.h"
#include "Light.h"
#include "MathConvert.h"

using namespace DirectX;
using namespace std;

IF::Scene::Scene(int winWidth, int winHeight, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
	:winWidth(winWidth), winHeight(winHeight), device(device), commandList(commandList), viewport(viewport)
{
	Graphic::SetDevice(device);
	Texture::setDevice(device);
	Model::SetDevice(device);
}

IF::Scene::~Scene()
{
	delete matPro;
	delete light;
	sound->Reset();
	sound->SoundUnLoad(testSound);
}

void IF::Scene::Initialize()
{
	//音源
	testSound = sound->LoadWave("Resources/Alarm01.wav");

	//光源設定
	light = LightManager::GetInstance();
	light->Initialize();
	light->DefaultLightSetting();
	for (int i = 0; i < 3; i++)
	{
		light->SetDirLightActive(i, true);
		light->SetPointLightActive(i, false);
		light->SetSpotLightActive(i, false);
	}
	light->SetCircleShadowActive(0, true);
	light->SetCircleShadowActive(1, true);
	light->SetCircleShadowActive(2, false);
	light->SetAmbientColor({ 1, 1, 1 });
	Object::SetLight(light);
	//定数バッファの初期化
	cb.Initialize(device);

	//画像関連初期化
	graph.Initialize(tex->descRangeSRV, L"ModelVS.hlsl", L"ModelPS.hlsl", L"ModelGS.hlsl");
	graph.Initialize2D(tex->descRangeSRV, L"SpriteVS.hlsl", L"SpritePS.hlsl");

	//モデルの読み込みとオブジェクトとの紐付け(空と地面)
	tex->Initialize();
	domeM.LoadModel("skydome");
	groundM.LoadModel("ground");
	domeObj.Initialize(device, &domeM);
	groundObj.Initialize(device, &groundM);
	groundObj.position = { 0,-2,0 };

	//カメラ関連初期化
	matPro = new Projection(45.0f, (float)winWidth, (float)winHeight);
	matView.eye = { 0,7.5,20 };

	//そのほかの初期化
	Rand::Initialize();

	sphereM.LoadModel("sphere", true);
	sphereO.Initialize(device, &sphereM);
	cube.LoadModel("cube", true);

	sphereO.position = { -1,0,0 };
	sphereO.scale = { 0.5,0.5,0.5 };
	matView.Update();
	player.Initialize(&cube, matView.Get(), matPro->Get(), matView.eye, device);

	//2D関連
	sprite.StaticInitialize(device, commandList, (float)winWidth, (float)winHeight);
	SGraph = tex->LoadTexture("Resources/texture.png");
	sprite.Initialize(SGraph, { 300,300 });

	sound->SoundPlay(testSound);

	//デバッグ用
#ifdef _DEBUG
	dText.Initialize(tex->LoadTexture("Resources/debugfont.png"));

#endif // _DEBUG
}

void IF::Scene::Update()
{
	Input* input = Input::getInstance();
	input->Update();

	//光源
	static XMFLOAT3 dlColor = { 1,1,1 };

	const static float cameraR = 20.0f;
	static float rota = -90.0f;

	if (input->Judge(KEY::Arrow, KEY::OR))
	{
		if (input->KDown(KEY::DOWN))matView.eye.z += 0.3f;
		if (input->KDown(KEY::UP))matView.eye.z -= 0.3f;
		if (input->KDown(KEY::RIGHT))matView.eye.x -= 0.3f;
		if (input->KDown(KEY::LEFT))matView.eye.x += 0.3f;
	}
	if (input->KDown(KEY::Z))rota++;
	if (input->KDown(KEY::C))rota--;
	if (rota < 0)rota += 360;
	if (rota > 360)rota -= 360;

	float rotaR = ConvertToRadians(rota);

	matView.target.x = matView.eye.x + cameraR * cosf(rotaR);
	matView.target.z = matView.eye.z + cameraR * sinf(rotaR);

	matView.Update();

	//if (input->KDown(KEY::W))lightDir.m128_f32[1] += 1.0f;
	//if (input->KDown(KEY::S))lightDir.m128_f32[1] -= 1.0f;
	//if (input->KDown(KEY::D))lightDir.m128_f32[0] += 1.0f;
	//if (input->KDown(KEY::A))lightDir.m128_f32[0] -= 1.0f;

	for (int i = 0; i < 2; i++)
	{
		light->SetCircleShadowDir(i, { csDir.x,csDir.y,csDir.z,0 });
		light->SetCircleShadowAtten(i, csAtten);
		light->SetCircleShadowFactorAngle(i, csAngle);
	}

	for (int i = 0; i < 3; i++)light->SetDirLightColor(i, dlColor);


	//matView.target = cubes[0].position;
	//matView.eye.x = cubes[0].position.x + front.x * 20;
	//matView.eye.z = cubes[0].position.z + front.z * 20;

	//matView.Update();

	domeObj.Update(matView.Get(), matPro->Get(), matView.eye);
	groundObj.Update(matView.Get(), matPro->Get(), matView.eye);
	sphereO.Update(matView.Get(), matPro->Get(), matView.eye);

	player.Update(matView.Get(), matPro->Get(), matView.eye, matView.target);

	light->SetCircleShadowCasterPos(0, player.GetPos());
	light->SetCircleShadowCasterPos(1, sphereO.position);

	light->Update();

	sprite.position = { 540,500 };
	sprite.Update();

	//デバッグ用
#ifdef _DEBUG
	static XMFLOAT3 front = player.GetFront();
	dText.Print(50, 50, 1.5, "front(x:%f,y:%f,z:%f)", front.x, front.y, front.z);

#endif // _DEBUG
}

void IF::Scene::Draw()
{
	graph.DrawBlendMode(commandList);
	domeObj.DrawBefore(commandList, graph.rootsignature.Get(), cb.GetGPUAddress());
	domeObj.Draw(commandList, viewport);

	groundObj.Draw(commandList, viewport);

	sphereO.Draw(commandList, viewport);

	player.Draw(commandList, viewport);

	graph.DrawBlendMode(commandList, Blend::NORMAL2D);
	sprite.DrawBefore(graph.rootsignature.Get(), cb.GetGPUAddress());
	//sprite.Draw(viewport);

	//デバッグ用
#ifdef _DEBUG
	dText.Draw(viewport);

#endif // _DEBUG
}
