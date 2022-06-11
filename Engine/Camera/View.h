#pragma once
#include <DirectXMath.h>

namespace IF
{
	class View
	{
		using XMMATRIX = DirectX::XMMATRIX;
		using XMFLOAT3 = DirectX::XMFLOAT3;
	private:
		XMMATRIX matView;

	public:
		static XMMATRIX matBillBoard;
		static XMMATRIX matBillBoardY;
		XMFLOAT3 eye;
		XMFLOAT3 target;
		XMFLOAT3 up;
	public:
		View(XMFLOAT3 eye = { 0,0,-100 }, XMFLOAT3 target = { 0,0,1 }, XMFLOAT3 up = { 0,1,0 });
		void Initialze(XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up);
		void Update();
		XMMATRIX Get()const;
	};
}