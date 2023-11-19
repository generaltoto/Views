#pragma once

static constexpr int SWAP_CHAIN_BUFFER_COUNT = 2;

/*
Base render class for the D3D12 API.
It contains all the necessary components and methods to use the D3D12 API.
This class is a singleton, you can get any DX12 component from it.
*/
class VGHandler final
{
public:
	enum class D3DState
	{
		SLEEP,
		INIT,
		RUNTIME
	};

	~VGHandler();

	/********** SINGLETON & STATE **********/
	static VGHandler* GetInstance();
	[[nodiscard]] D3DState GetState() const { return m_State; }
	void SetState(const D3DState state) { m_State = state; }
private:
	static VGHandler* m_pApp;
	D3DState m_State;
	/********** SINGLETON & STATE **********/
	
public:
	/*************** BASIC *****************/
	void Init(const Win32::Window* window);
	void Update(const float dt, const float totalTime);
	void Render();
	void OnResize(int, int);
	/*************** BASIC *****************/

	/************** GET/SET ****************/
	[[nodiscard]] ID3D12Device* GetDevice() const { return m_pDevice; }
	[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList; }
	UINT GetCbvHeap(ID3D12DescriptorHeap** heap) const;
	/************** GET/SET ****************/

	/************** CMD LIST ***************/
	void BeginList() const;
	void EndList();
	/************** CMD LIST ***************/
	
private:
	VGHandler();
	
	/********** DEBUG MANAGEMENT **********/
	void EnableDebugLayer();
	void CreateInfoQueue() const;
	ID3D12Debug* m_pDebugController;
	/********** DEBUG MANAGEMENT **********/

	/*************** DEVICE ***************/
	void CreateDevice();
	IDXGIFactory4* m_pDxgiFactory;
	ID3D12Device* m_pDevice;
	/*************** DEVICE ***************/

	/**************** MSAA ****************/
	void CheckMsaaQualitySupport();
	bool m_4XMsaaState;
	UINT m_4XMsaaQuality;
	/**************** MSAA ****************/

	/********** FENCE & DESC SIZE *********/
	void CreateFenceAndGetDescriptorsSizes();
	ID3D12Fence* m_pFence;
	int m_CurrentFenceValue;
	
	UINT m_RtvDescriptorSize;
	UINT m_DsvDescriptorSize;
	UINT m_CbvSrvUavDescriptorSize;
	/********** FENCE & DESC SIZE *********/

	/*********** COMMAND QUEUE ************/
	void CreateCommandObjects();
	void FlushCommandQueue();
	ID3D12CommandQueue* m_pCommandQueue;
	ID3D12CommandAllocator* m_pCommandAllocator;
	ID3D12GraphicsCommandList* m_pCommandList;
	/*********** COMMAND QUEUE ************/

	/************ SWAP CHAIN **************/
	void CreateSwapChain(HWND windowHandle);
	void CreateRenderTargetView();
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	IDXGISwapChain* m_pSwapChain;
	ID3D12Resource* m_pSwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
	DXGI_FORMAT m_BackBufferFormat;
	int m_CurrBackBuffer;
	/************ SWAP CHAIN **************/

	/******* DEPTH STENCIL BUFFER *********/
	void CreateDepthStencilBuffer();
	[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;
	ID3D12Resource* m_pDepthStencilBuffer;
	DXGI_FORMAT m_DepthStencilFormat;
	/******* DEPTH STENCIL BUFFER *********/
	
	/*************** HEAPS ****************/
	void CreateRtvAndDsvDescriptorHeaps();
	ID3D12DescriptorHeap* m_pRtvHeap;
	ID3D12DescriptorHeap* m_pDsvHeap;
	ID3D12DescriptorHeap* m_pCbvSrvHeap;
	/*************** HEAPS ****************/
	
	void CreateResources();

	int m_BufferWidth;
	int m_BufferHeight;
};
