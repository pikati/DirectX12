#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "Utility.h"
#include "MyDirect3D.h"
#include "DirectX12App.h"

using namespace std;

#pragma region globalVariable
static WNDCLASSEX w;
static HWND hwnd;
static DirectX12App* application;
#pragma endregion


void CreateWindowClass();
void MessageLoop();


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CreateWindowClass();
	application = new DirectX12App();
	application->Initialize(hwnd);
	MessageLoop();
	delete application;
	return 0;
}

//ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//ウィンドウ破棄が実行されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	//OSに終了を伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	//既定の処理を行う
}

void CreateWindowClass() 
{
	w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//コールバック関数の指定
	w.lpszClassName = ("DirectX12Test");		//アプリケーションクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ハンドルの取得

	RegisterClassEx(&w);	//アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）

	RECT rect = { 0,0,WINDOW_WIDTH, WINDOW_HEIGHT };	//ウィンドウのサイズ指定

	//ウィンドウサイズの補正
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindow(w.lpszClassName, //クラス名指定
		"DirectX12",			//タイトルバーの指定
		WS_OVERLAPPEDWINDOW,	//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,			//表示するx座標
		CW_USEDEFAULT,			//表示するy座標
		rect.right - rect.left,	//ウィンドウ幅
		rect.bottom - rect.top, //ウィンドウ高
		nullptr,				//親ウィンドウハンドル
		nullptr,				//メニューハンドル
		w.hInstance,			//呼び出しアプリケーションハンドル
		nullptr					//追加パラメータ
	);

	ShowWindow(hwnd, SW_SHOW);
}

void MessageLoop() {
	MSG msg = {  };

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		application->Update();
		application->Draw();

		//アプリケーションが終わるときにmessageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	//もうクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);
}