#include "DrawManager.h"

DrawManager::DrawManager() 
{
	m_cmdAllocator = nullptr;
	m_cmdList = nullptr;
	m_cmdQueue = nullptr;
	m_rtvHeaps = nullptr;
	m_device = nullptr;
	m_factory = nullptr;
	m_hwnd = nullptr;
	m_chain = nullptr;
	m_fence = nullptr;
}

DrawManager::~DrawManager() 
{

}

void DrawManager::Initialize(ID3D12Device* device, IDXGIFactory6* factory, HWND hwnd, IDXGISwapChain4* chain)
{
	m_device = device;
	m_factory = factory;
	m_hwnd = hwnd;
	m_chain = chain;
	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator));
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator, nullptr, IID_PPV_ARGS(&m_cmdList));
	InitializeFence();
	InitializeQueue();
	InitializeSwapChain();
	InitializeDescriptorHeap();
	InitializeRenderTargetView();
	InitializeRenderTarget();
}

void DrawManager::Draw()
{
	Clear();
	FlipRenderTarget();
	//0だと垂直同期を待たずに次のフレームが始まる1にすると待つよ
	m_chain->Present(1, 0);
}

void DrawManager::Finalize()
{

}

void DrawManager::InitializeQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	//タイムアウトなし
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	//アダプターを1つしか使わないときは0
	cmdQueueDesc.NodeMask = 0;

	//プライオリティは指定なしで
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	//コマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	//キュー生成
	m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue));
}

void DrawManager::InitializeSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 sChainDesc = {};

	sChainDesc.Width = WINDOW_WIDTH;
	sChainDesc.Height = WINDOW_HEIGHT;
	sChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sChainDesc.Stereo = false;
	sChainDesc.SampleDesc.Count = 1;
	sChainDesc.SampleDesc.Quality = 0;
	sChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	sChainDesc.BufferCount = 2;

	//バックバッファは伸び縮み可能
	sChainDesc.Scaling = DXGI_SCALING_STRETCH;

	//フリップ後は速やかに破棄
	sChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//特に指定なし
	sChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	//ウィンドウとフルスクリーンの切り替え可能
	sChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT h = m_factory->CreateSwapChainForHwnd(
		m_cmdQueue,
		m_hwnd,
		&sChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&m_chain
		);
}

void DrawManager::InitializeDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビューなのでRTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; //表裏の2つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //指定なし FLAG_SHADER_VISIBLEとかにするとシェーダーとか使える
	
	m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeaps));
}

void DrawManager::InitializeRenderTargetView()
{
	DXGI_SWAP_CHAIN_DESC sChainDesc = {};

	m_chain->GetDesc(&sChainDesc);

	m_backBuffers.resize(sChainDesc.BufferCount);
	for (int idx = 0; idx < sChainDesc.BufferCount; idx++)
	{
		//スワップチェーン内のバッファとビューの関連付けをする（２つバッファなので２回繰り返すよ）
		m_chain->GetBuffer(idx, IID_PPV_ARGS(&m_backBuffers[idx]));

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += idx * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_device->CreateRenderTargetView(m_backBuffers[idx], nullptr, handle);
	}
}

void DrawManager::InitializeFence()
{
	m_fenceVal = 0;
	auto result = m_device->CreateFence(m_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
}

void DrawManager::InitializeRenderTarget()
{
	m_barrierDesc = {};

	m_barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	m_barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	m_barrierDesc.Transition.pResource = m_backBuffers[bbIdx];
	m_barrierDesc.Transition.Subresource = 0;
}

////////////////////////////////////////////86p\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
void DrawManager::Clear()
{
	m_cmdList->Reset(m_cmdAllocator, );
	HRESULT h = m_cmdAllocator->Reset();
	//これ複数回使うからconstで保存しておくべくとか
	bbIdx = m_chain->GetCurrentBackBufferIndex();//バックバッファのインデックスを取得ここに描画していく

	auto rtvH = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	SetRenderTarget();

	m_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	m_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	m_cmdList->Close();

	ID3D12CommandList* cmdLists[] = { m_cmdList };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);
	
	//ここで処理終わったかチェックしてる
	UINT64 n = m_fence->GetCompletedValue();
	m_cmdQueue->Signal(m_fence, ++m_fenceVal);
	while (n != m_fenceVal)
	{
		//イベントハンドルの取得
		auto event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);

		//イベントが発生するまで待ち続ける
		WaitForSingleObject(event, INFINITE);

		//イベントハンドルを閉じる
		CloseHandle(event);
	}

	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator, nullptr);
}

void DrawManager::SetRenderTarget()
{
	m_barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	m_barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_cmdList->ResourceBarrier(1, &m_barrierDesc);
}

void DrawManager::FlipRenderTarget()
{
	m_barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	m_cmdList->ResourceBarrier(1, &m_barrierDesc);
}