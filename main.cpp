#include "Window.h"
#include "DirectX12.h"
#include "Input.h"
#include "Scene.h"
#include "FPS.h"
#include "Sound.h"
#ifdef _DEBUG
#include "Debug.h"
#endif // _DEBUG

using namespace IF;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	// ウィンドウサイズ
	const int winWidth = 1280;  // 横幅
	const int winHeight = 720;  // 縦幅

	Window::Instance()->Initialize(winWidth, winHeight);
#ifdef _DEBUG
	Debug();
#endif // _DEBUG

	DirectX12::Instance()->Initialize(Window::Instance()->hwnd, winWidth, winHeight);
	Input::getInstance()->Initialize(Window::Instance()->w.hInstance, Window::Instance()->hwnd);
	LightManager::GetInstance()->SetDevice(DirectX12::Instance()->device.Get());
	Sound::Instance()->Initialize();
	IScene* scene = new Scene(winWidth, winHeight, DirectX12::Instance()->device.Get(),
		DirectX12::Instance()->commandList.Get(), DirectX12::Instance()->viewport);
	DirectX12::Instance()->SetClearColor(0, 0, 0);
	scene->Initialize();
	FPS fps;
	fps.Initialize(60);

	while (!Input::getInstance()->KDown(KEY::ESC))
	{
		//メッセージ
		if (Window::Instance()->Message())break;

		scene->Update();

		DirectX12::Instance()->DrawBefore();
		scene->Draw();
		DirectX12::Instance()->DrawAfter();
		fps.FPSFixed();
	}
	delete scene;
	Window::Instance()->Unregister();
	return 0;
}