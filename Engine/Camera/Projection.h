#pragma once
#include <DirectXMath.h>

namespace IF
{
	class Projection
	{
		using XMMATRIX = DirectX::XMMATRIX;
	private:
		XMMATRIX matProjection;
	public:
		float nearZ, farZ;
		float winWidth, winHeight;
		float fovAngle;

	public:
		Projection(float radian, float winWidth, float winHeight, float nearZ = 0.1f, float farZ = 1000.0f);
		void Inisialize(float radian, float winWidth, float winHeight, float nearZ = 0.1f, float farZ = 1000.0f);
		void Update();
		XMMATRIX Get()const;
	};
}