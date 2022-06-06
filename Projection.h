#pragma once
#include <DirectXMath.h>

namespace IF
{
	class Projection
	{
		using XMMATRIX = DirectX::XMMATRIX;
	private:
		XMMATRIX matProjection;
		float nearZ, farZ;

	public:
		Projection(float radian, float winWidth, float winHeight, float nearZ = 0.1f, float farZ = 1000.0f);
		void Inisialize(float radian, float winWidth, float winHeight, float nearZ = 0.1f, float farZ = 1000.0f);

		XMMATRIX Get()const;
	};
}