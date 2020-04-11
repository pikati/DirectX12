#include "DirectX12App.h"

DirectX12App::DirectX12App() 
{
	m_device = nullptr;
	m_factory = nullptr;
	m_chain = nullptr;
	m_hwnd = nullptr;
	m_drawManager = nullptr;
}

DirectX12App::~DirectX12App() 
{

}

void DirectX12App::Initialize(HWND hwnd)
{
#ifdef _DEBUG
	EnableDebugLayer();
#endif // _DEBUG

	
	CreateDevice();
	m_drawManager = new DrawManager();
	m_hwnd = hwnd;
	m_drawManager->Initialize(m_device, m_factory, m_hwnd, m_chain);
}

void DirectX12App::Update()
{

}

void DirectX12App::Draw() 
{
	m_drawManager->Draw();
}

void DirectX12App::Finalize()
{

}

void DirectX12App::CreateDevice() 
{
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL fLevel;

	for (auto lv : levels)
	{
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&m_device)) == S_OK)
		{
			fLevel = lv;
			break;
		}
	}

	if (m_device == nullptr)
	{
		//アプリケーション終了処理
	}

#ifdef _DEBUG
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG ,IID_PPV_ARGS(&m_factory));
#else
	CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
#endif // _DEBUG


	if (m_factory == nullptr)
	{
		//アプリケーション終了処理
	}
}

void DirectX12App::EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}

ID3D12Device* DirectX12App::GetDevice() 
{
	return m_device;
}