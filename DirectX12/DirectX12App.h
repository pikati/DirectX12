#pragma once
#include "MyDirect3D.h"
#include "DrawManager.h"

class DirectX12App
{

private:
	ID3D12Device* m_device;
	IDXGIFactory6* m_factory;	//�O���t�B�b�N�{�[�h�̗񋓂Ɏg�p����Ǝv����
	IDXGISwapChain4* m_chain;
	DrawManager* m_drawManager;
	HWND	m_hwnd;

	void CreateDevice();
	void EnableDebugLayer();
public:
	DirectX12App();
	~DirectX12App();

	void Initialize(HWND hwnd);
	void Update();
	void Draw();
	void Finalize();

	ID3D12Device* GetDevice();
};



