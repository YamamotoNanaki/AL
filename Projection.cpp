#include "Projection.h"
using namespace DirectX;
using namespace IF;

Projection::Projection(float radian, float winWidth, float winHeight, float nearZ, float farZ)
{
	Inisialize(radian, winWidth, winHeight, nearZ, farZ);
}

void Projection::Inisialize(float radian, float winWidth, float winHeight, float nearZ, float farZ)
{
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(radian), (float)winWidth / winHeight, nearZ, farZ);
}

XMMATRIX Projection::Get() const
{
	return matProjection;
}
