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

	sphereO.position = { -1,0,0 };
	sphereO.scale = { 0.5,0.5,0.5 };
	matView.Update();

	cube.LoadModel("cube", true);
	for (int i = 0; i < _countof(cubes); i++)
	{
		cubes[i].Initialize(device, &cube);
		if (i > 0)
		{
			cubes[i].parent = &cubes[i - 1];
			cubes[i].scale = { 0.7f,0.7f,0.7f };
			cubes[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			cubes[i].position = { 0.0f,0.0f,-1 };
		}
		else cubes[i].position.y = 1;

		cubes[i].Update(matView.Get(), matPro->Get(), matView.eye);
	}


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

	if (input->Judge(KEY::WASD, KEY::OR))
	{
		if (input->KDown(KEY::S))matView.eye.z += 0.3f, matView.target.z += 0.3f;
		if (input->KDown(KEY::W))matView.eye.z -= 0.3f, matView.target.z -= 0.3f;
		if (input->KDown(KEY::D))matView.eye.x -= 0.3f, matView.target.x -= 0.3f;
		if (input->KDown(KEY::A))matView.eye.x += 0.3f, matView.target.x += 0.3f;
	}
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

	XMFLOAT3 front = { 0,0,0 };
	XMFLOAT3 move = { 0,0,0 };
	float rota = 0;

	const float kCharacterSpeed = 0.2f;
	const float kRotaSpeed = 0.01;

	if (input->KDown(KEY::RIGHT))
	{
		rota += kRotaSpeed;
	}
	if (input->KDown(KEY::LEFT))
	{
		rota -= kRotaSpeed;
	}
	cubes[0].rotation.y += rota;

	front = { sinf(cubes[0].rotation.y),0, cosf(cubes[0].rotation.y) };


	if (input->KDown(KEY::UP))
	{
		move = { front.x * -kCharacterSpeed, 0, front.z * -kCharacterSpeed };
	}
	if (input->KDown(KEY::DOWN))
	{
		move = { front.x * kCharacterSpeed, 0, front.z * kCharacterSpeed };
	}

	cubes[0].position.x += move.x;
	cubes[0].position.y += move.y;
	cubes[0].position.z += move.z;

	//matView.target = cubes[0].position;
	//matView.eye.x = cubes[0].position.x + front.x * 20;
	//matView.eye.z = cubes[0].position.z + front.z * 20;

	//matView.Update();

	for (int i = 0; i < _countof(cubes); i++)
	{
		cubes[i].Update(matView.Get(), matPro->Get(), matView.eye);
	}


	domeObj.Update(matView.Get(), matPro->Get(), matView.eye);
	groundObj.Update(matView.Get(), matPro->Get(), matView.eye);
	sphereO.Update(matView.Get(), matPro->Get(), matView.eye);

	light->SetCircleShadowCasterPos(0, cubes[0].position);
	light->SetCircleShadowCasterPos(1, sphereO.position);

	light->Update();

	sprite.position = { 540,500 };
	sprite.Update();

	//デバッグ用
#ifdef _DEBUG
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

	for (int i = 0; i < _countof(cubes); i++)cubes[i].Draw(commandList, viewport);

	graph.DrawBlendMode(commandList, Blend::NORMAL2D);
	sprite.DrawBefore(graph.rootsignature.Get(), cb.GetGPUAddress());
	//sprite.Draw(viewport);

	//デバッグ用
#ifdef _DEBUG
	dText.Draw(viewport);

#endif // _DEBUG
}
