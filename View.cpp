#include "View.h"

using namespace DirectX;
using namespace IF;

XMMATRIX View::matBillBoard = XMMatrixIdentity();
XMMATRIX View::matBillBoardY = XMMatrixIdentity();

View::View(XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up)
{
	Initialze(eye, target, up);

}

void View::Initialze(XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	Update();
}

void View::Update()
{
	//視点座標、注視点座標、上方向
	XMVECTOR eyePosition = XMLoadFloat3(&eye);
	XMVECTOR targetPosition = XMLoadFloat3(&target);
	XMVECTOR upVector = XMLoadFloat3(&up);

	//カメラZ軸
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	//ゼロベクトルを除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	//ベクトル正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//カメラX軸Y軸
	XMVECTOR cameraAxisX;
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	cameraAxisX = XMVector3Normalize(cameraAxisX);
	XMVECTOR cameraAxisY;
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	//回転行列の作成
	XMMATRIX matCameraRot;
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0, 0, 0, 1);

	//逆行列を代入
	matView = XMMatrixTranspose(matCameraRot);

	//カメラの位置から原点へのベクトルを生成
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	matView.r[3] = translation;

	matBillBoard.r[0] = cameraAxisX;
	matBillBoard.r[1] = cameraAxisY;
	matBillBoard.r[2] = cameraAxisZ;
	matBillBoard.r[3] = XMVectorSet(0, 0, 0, 1);

	//Y軸ビルボード
	XMVECTOR yBillAxisX, yBillAxisY, yBillAxisZ;

	yBillAxisX = cameraAxisX;
	yBillAxisY = XMVector3Normalize(upVector);
	yBillAxisZ = XMVector3Cross(yBillAxisX, yBillAxisY);

	matBillBoardY.r[0] = yBillAxisX;
	matBillBoardY.r[1] = yBillAxisY;
	matBillBoardY.r[2] = yBillAxisZ;
	matBillBoardY.r[3] = XMVectorSet(0, 0, 0, 1);
}

XMMATRIX View::Get() const
{
	return matView;
}
