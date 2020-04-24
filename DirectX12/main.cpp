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

//�E�B���h�E�v���V�[�W��
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//�E�B���h�E�j�������s���ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	//OS�ɏI����`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	//����̏������s��
}

void CreateWindowClass() 
{
	w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	//�R�[���o�b�N�֐��̎w��
	w.lpszClassName = ("DirectX12Test");		//�A�v���P�[�V�����N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�n���h���̎擾

	RegisterClassEx(&w);	//�A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w���OS�ɓ`����j

	RECT rect = { 0,0,WINDOW_WIDTH, WINDOW_HEIGHT };	//�E�B���h�E�̃T�C�Y�w��

	//�E�B���h�E�T�C�Y�̕␳
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(w.lpszClassName, //�N���X���w��
		"DirectX12",			//�^�C�g���o�[�̎w��
		WS_OVERLAPPEDWINDOW,	//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,			//�\������x���W
		CW_USEDEFAULT,			//�\������y���W
		rect.right - rect.left,	//�E�B���h�E��
		rect.bottom - rect.top, //�E�B���h�E��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		w.hInstance,			//�Ăяo���A�v���P�[�V�����n���h��
		nullptr					//�ǉ��p�����[�^
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

		//�A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	//�����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(w.lpszClassName, w.hInstance);
}