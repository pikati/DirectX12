#pragma once
#include "MyDirect3D.h"
#include <vector>

class DrawManager
{
private:
	ID3D12CommandAllocator* m_cmdAllocator;
	ID3D12GraphicsCommandList* m_cmdList;
	ID3D12CommandQueue* m_cmdQueue;
	ID3D12DescriptorHeap* m_rtvHeaps;
	std::vector<ID3D12Resource*> m_backBuffers;
	D3D12_RESOURCE_BARRIER m_barrierDesc;
	unsigned int bbIdx;

	//待つための仕組みフェンスだよ
	ID3D12Fence* m_fence;
	UINT64 m_fenceVal;

	//この四つは変数にしないで直接取得するのでもいいかも
	ID3D12Device* m_device;
	IDXGIFactory6* m_factory;
	IDXGISwapChain4* m_chain;
	HWND	m_hwnd;

	void InitializeQueue();
	void InitializeSwapChain();
	void InitializeDescriptorHeap();
	void InitializeRenderTargetView();

	void InitializeFence();
	void InitializeBarrier();

	void SetRenderTarget();
	void FlipRenderTarget(bool b);

	void WaitForCommandQueue();
public:
	DrawManager();
	~DrawManager();
	void Initialize(ID3D12Device* device, IDXGIFactory6* factory, HWND hwnd, IDXGISwapChain4* chain);
	void Draw();
	void Finalize();

	void Clear();
};

