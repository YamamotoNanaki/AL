#include "Player.h"
#include "Input.h"

using namespace IF;
using namespace DirectX;
using namespace std;

void Player::Update(XMMATRIX matView, XMMATRIX matProjection, XMFLOAT3 cameraPos, XMFLOAT3 cameraTarget)
{
	static bool moveFlag = false;

	const float kCharacterSpeed = 0.2f;

	Input* input = Input::getInstance();

	if (input->KTriggere(KEY::Q))moveFlag = moveFlag == false;

	if (moveFlag)
	{
		XMFLOAT3 move = { 0,0,0 };
		float rota = 0;
		const float kRotaSpeed = 0.02f;

		if (input->KDown(KEY::D))
		{
			rota += kRotaSpeed;
		}
		if (input->KDown(KEY::A))
		{
			rota -= kRotaSpeed;
		}
		obj[0].rotation.y += rota;

		front = { sinf(obj[0].rotation.y),0, cosf(obj[0].rotation.y) };


		if (input->KDown(KEY::W))
		{
			move = { front.x * -kCharacterSpeed, 0, front.z * -kCharacterSpeed };
		}
		if (input->KDown(KEY::S))
		{
			move = { front.x * kCharacterSpeed, 0, front.z * kCharacterSpeed };
		}

		obj[0].position.x += move.x;
		obj[0].position.y += move.y;
		obj[0].position.z += move.z;
	}
	else
	{
		XMVECTOR cameraFront = { cameraTarget.x - cameraPos.x,0.0f,cameraTarget.z - cameraPos.z };
		cameraFront = XMVector3Normalize(cameraFront);

		XMVECTOR right = XMVector3Cross({ 0,1,0 }, cameraFront);

		if (input->Judge(KEY::WASD, KEY::OR))
		{
			cameraFront *= kCharacterSpeed;
			right *= kCharacterSpeed;
			if (input->KDown(KEY::W))
			{
				obj[0].position.x += cameraFront.m128_f32[0];
				obj[0].position.y += cameraFront.m128_f32[1];
				obj[0].position.z += cameraFront.m128_f32[2];
			}
			if (input->KDown(KEY::S))
			{
				obj[0].position.x -= cameraFront.m128_f32[0];
				obj[0].position.y -= cameraFront.m128_f32[1];
				obj[0].position.z -= cameraFront.m128_f32[2];
			}
			if (input->KDown(KEY::A))
			{
				obj[0].position.x -= right.m128_f32[0];
				obj[0].position.y -= right.m128_f32[1];
				obj[0].position.z -= right.m128_f32[2];
			}
			if (input->KDown(KEY::D))
			{
				obj[0].position.x += right.m128_f32[0];
				obj[0].position.y += right.m128_f32[1];
				obj[0].position.z += right.m128_f32[2];
			}
		}
	}

	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Update(matView, matProjection, cameraPos);
	}
}

void Player::Draw(ID3D12GraphicsCommandList* commandList, vector<D3D12_VIEWPORT> viewport)
{
	for (int i = 0; i < _countof(obj); i++)obj[i].Draw(commandList, viewport);
}

void Player::Initialize(Model* model, DirectX::XMMATRIX matView, DirectX::XMMATRIX matProjection, DirectX::XMFLOAT3 cameraPos, ID3D12Device* device)
{
	for (int i = 0; i < _countof(obj); i++)
	{
		obj[i].Initialize(device, model);
		if (i > 0)
		{
			obj[i].parent = &obj[i - 1];
			obj[i].scale = { 0.7f,0.7f,0.7f };
			obj[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
			obj[i].position = { 0.0f,0.0f,-1 };
		}
		else obj[i].position.y = 1;

		obj[i].Update(matView, matProjection, cameraPos);
	}
}

DirectX::XMFLOAT3 Player::GetFront() const
{
	return front;
}

DirectX::XMFLOAT3 Player::GetPos() const
{
	return obj[0].position;
}
