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
	//0���Ɛ���������҂����Ɏ��̃t���[�����n�܂�1�ɂ���Ƒ҂�
	m_chain->Present(1, 0);
}

void DrawManager::Finalize()
{

}

void DrawManager::InitializeQueue()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	//�^�C���A�E�g�Ȃ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	//�A�_�v�^�[��1�����g��Ȃ��Ƃ���0
	cmdQueueDesc.NodeMask = 0;

	//�v���C�I���e�B�͎w��Ȃ���
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	//�R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	//�L���[����
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

	//�o�b�N�o�b�t�@�͐L�яk�݉\
	sChainDesc.Scaling = DXGI_SCALING_STRETCH;

	//�t���b�v��͑��₩�ɔj��
	sChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//���Ɏw��Ȃ�
	sChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	//�E�B���h�E�ƃt���X�N���[���̐؂�ւ��\
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

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2; //�\����2��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; //�w��Ȃ� FLAG_SHADER_VISIBLE�Ƃ��ɂ���ƃV�F�[�_�[�Ƃ��g����
	
	m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeaps));
}

void DrawManager::InitializeRenderTargetView()
{
	DXGI_SWAP_CHAIN_DESC sChainDesc = {};

	m_chain->GetDesc(&sChainDesc);

	m_backBuffers.resize(sChainDesc.BufferCount);
	for (int idx = 0; idx < sChainDesc.BufferCount; idx++)
	{
		//�X���b�v�`�F�[�����̃o�b�t�@�ƃr���[�̊֘A�t��������i�Q�o�b�t�@�Ȃ̂łQ��J��Ԃ���j
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
	//���ꕡ����g������const�ŕۑ����Ă����ׂ��Ƃ�
	bbIdx = m_chain->GetCurrentBackBufferIndex();//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾�����ɕ`�悵�Ă���

	auto rtvH = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	SetRenderTarget();

	m_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	m_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	m_cmdList->Close();

	ID3D12CommandList* cmdLists[] = { m_cmdList };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);
	
	//�����ŏ����I��������`�F�b�N���Ă�
	UINT64 n = m_fence->GetCompletedValue();
	m_cmdQueue->Signal(m_fence, ++m_fenceVal);
	while (n != m_fenceVal)
	{
		//�C�x���g�n���h���̎擾
		auto event = CreateEvent(nullptr, false, false, nullptr);
		m_fence->SetEventOnCompletion(m_fenceVal, event);

		//�C�x���g����������܂ő҂�������
		WaitForSingleObject(event, INFINITE);

		//�C�x���g�n���h�������
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