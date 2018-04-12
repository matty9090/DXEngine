#include "Graphics.hpp"

#include <sstream>

Graphics::Graphics(HWND hwnd, size_t windowW, size_t windowH) : m_Hwnd(hwnd), m_WindowW(windowW), m_WindowH(windowH),
	m_Near(0.001f), m_Far(1000.0f), m_Wireframe(false) 
{
	m_SwapChain = NULL;
	m_Device = NULL;
	m_DeviceContext = NULL;
	m_RenderTargetView = NULL;
	m_DepthStencilBuffer = NULL;
	m_DepthStencilStateOn = NULL;
	m_DepthStencilStateOff = NULL;
	m_DepthStencilView = NULL;
	m_RasterState = NULL;
}

Graphics::~Graphics() {

}

bool Graphics::init() { 
	if (!initAdapter())     { MessageBoxA(m_Hwnd, "Failed to init adapter",     "Error", MB_OK); return false; }
	if (!initDevice())      { MessageBoxA(m_Hwnd, "Failed to init device",      "Error", MB_OK); return false; }
	if (!initBackBuffer())  { MessageBoxA(m_Hwnd, "Failed to init back buffer", "Error", MB_OK); return false; }
	if (!initDepthBuffer()) { MessageBoxA(m_Hwnd, "Failed to init buffer",      "Error", MB_OK); return false; }
	if (!initRaster())      { MessageBoxA(m_Hwnd, "Failed to init raster",      "Error", MB_OK); return false; }
	if (!initViewport())    { MessageBoxA(m_Hwnd, "Failed to init viewport",    "Error", MB_OK); return false; }
	if (!initMatrices())    { MessageBoxA(m_Hwnd, "Failed to init matrices",    "Error", MB_OK); return false; }
	if (!initFonts())		{ MessageBoxA(m_Hwnd, "Failed to init fonts",	    "Error", MB_OK); return false; }
	
	return true;
}

void Graphics::beginScene() {
	float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_DeviceContext->RSSetViewports(1, &m_Viewport);
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, colour);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::endScene() {
	m_SwapChain->Present(0, 0);
}

void Graphics::toggleWireframe() {
	m_Wireframe = !m_Wireframe;
	initRaster(m_Wireframe);
}

void Graphics::createMirror(Mirror &mirror) {
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width  = mirror.res.x;
	texDesc.Height = mirror.res.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	m_Device->CreateTexture2D(&texDesc, NULL, &mirror.tex);
	m_Device->CreateRenderTargetView(mirror.tex, NULL, &mirror.target);

	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	srv.Format = texDesc.Format;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MostDetailedMip = 0;
	srv.Texture2D.MipLevels = 1;

	m_Device->CreateShaderResourceView(mirror.tex, &srv, &mirror.map);

	texDesc.Width = mirror.res.x;
	texDesc.Height = mirror.res.y;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	m_Device->CreateTexture2D(&texDesc, NULL, &mirror.stencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencil;
	depthStencil.Format = texDesc.Format;
	depthStencil.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencil.Texture2D.MipSlice = 0;
	depthStencil.Flags = 0;

	HRESULT res = m_Device->CreateDepthStencilView(mirror.stencil, &depthStencil, &mirror.depthStencil);

	D3D11_VIEWPORT vp;
	vp.Width = (float)mirror.res.x;
	vp.Height = (float)mirror.res.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	mirror.viewport = vp;
}

void Graphics::setRenderTargets(ID3D11RenderTargetView *target, ID3D11DepthStencilView *depth, D3D11_VIEWPORT &viewport) {
	float colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	m_DeviceContext->RSSetViewports(1, &viewport);
	m_DeviceContext->OMSetRenderTargets(1, &target, depth);
	m_DeviceContext->ClearRenderTargetView(target, colour);
	m_DeviceContext->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::drawText(float x, float y, UINT32 colour, std::string str) {
	size_t conv;
	const size_t size = str.length() + 1;
	wchar_t *wc = new wchar_t[size];
	mbstowcs_s(&conv, wc, size, str.c_str(), size);

	m_FontWrapper->DrawString(m_DeviceContext, wc, 22.0f, x, y, colour, FW1_RESTORESTATE);
}

void Graphics::drawTextValue(float x, float y, std::string desc, float value) {
	std::ostringstream ss;
	ss.precision(5);
	ss << desc << ": " << value;

	drawText(x, y, D3DCOLOR_ARGB(255, 255, 255, 255), ss.str());
}

void Graphics::drawTextValue(float x, float y, std::string desc, D3DXVECTOR3 value) {
	std::ostringstream ss;
	ss.precision(5);
	ss << desc << ": (" << value.x << ", " << value.y << ", " << value.z << ")";

	drawText(x, y, D3DCOLOR_ARGB(255, 255, 255, 255), ss.str());
}

bool Graphics::initAdapter() {
	IDXGIFactory *factory;
	IDXGIAdapter *adapter;
	IDXGIOutput *adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC *displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;

	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory))) return false;
	if (FAILED(factory->EnumAdapters(0, &adapter))) return false;
	if (FAILED(adapter->EnumOutputs(0, &adapterOutput))) return false;
	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL))) return false;

	displayModeList = new DXGI_MODE_DESC[numModes];

	if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList))) return false;

	for (i = 0; i<numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)m_WindowW) {
			if (displayModeList[i].Height == (unsigned int)m_WindowH) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	if (FAILED(adapter->GetDesc(&adapterDesc))) return false;

	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0) return false;

	delete[] displayModeList;

	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	return true;
}

bool Graphics::initDevice() {
	RECT rc;
	GetClientRect(m_Hwnd, &rc);
	m_ViewportW = rc.right - rc.left;
	m_ViewportH = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_WindowW;
	swapChainDesc.BufferDesc.Height = m_WindowH;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.OutputWindow = m_Hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext)))
		return false;

	return true;
}

bool Graphics::initBackBuffer() {
	ID3D11Texture2D *backBufferPtr;

	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
		return false;

	if (FAILED(m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_RenderTargetView)))
		return false;

	backBufferPtr->Release();

	return true;
}

bool Graphics::initDepthBuffer() {
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDescOn, depthStencilDescOff;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	ZeroMemory(&depthStencilDescOn, sizeof(depthStencilDescOn));
	ZeroMemory(&depthStencilDescOff, sizeof(depthStencilDescOff));
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthBufferDesc.Width = m_WindowW;
	depthBufferDesc.Height = m_WindowH;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if (FAILED(m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer)))
		return false;

	depthStencilDescOn.DepthEnable = true;
	depthStencilDescOn.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescOn.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDescOn.StencilEnable = true;
	depthStencilDescOn.StencilReadMask = 0xFF;
	depthStencilDescOn.StencilWriteMask = 0xFF;

	depthStencilDescOn.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescOn.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDescOn.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescOn.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDescOn.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescOn.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDescOn.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDescOn.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_Device->CreateDepthStencilState(&depthStencilDescOn, &m_DepthStencilStateOn)))
		return false;

	depthStencilDescOff.DepthEnable = true;
	depthStencilDescOff.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDescOff.DepthFunc = D3D11_COMPARISON_LESS;

	if (FAILED(m_Device->CreateDepthStencilState(&depthStencilDescOff, &m_DepthStencilStateOff)))
		return false;

	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilStateOn, 1);

	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView)))
		return false;

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	return true;
}

bool Graphics::initRaster(bool wireframe) {
	D3D11_RASTERIZER_DESC rasterDesc;

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = (wireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState)))
		return false;

	m_DeviceContext->RSSetState(m_RasterState);

	return true;
}

bool Graphics::initViewport() {
	m_Viewport.Width = (float)m_WindowW;
	m_Viewport.Height = (float)m_WindowH;
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;
	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	
	m_DeviceContext->RSSetViewports(1, &m_Viewport);

	return true;
}

bool Graphics::initMatrices() {
	float fov = (float)D3DX_PI / 4.0f;
	float aspect = (float)m_WindowW / (float)m_WindowH;

	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, fov, aspect, m_Near, m_Far);
	D3DXMatrixOrthoLH(&m_OrthoMatrix, (float)m_WindowW, (float)m_WindowH, m_Near, m_Far);

	return true;
}

bool Graphics::initFonts() {
	IFW1Factory *fw1factory;

	if (FAILED(FW1CreateFactory(FW1_VERSION, &fw1factory)))
		return false;
	
	return !FAILED(fw1factory->CreateFontWrapper(m_Device, L"Consolas", &m_FontWrapper));
}

void Graphics::cleanup() {
	if (m_SwapChain) m_SwapChain->SetFullscreenState(false, NULL);
	if (m_RasterState) m_RasterState->Release();
	if (m_DepthStencilView) m_DepthStencilView->Release();
	if (m_DepthStencilStateOn) m_DepthStencilStateOn->Release();
	if (m_DepthStencilStateOff) m_DepthStencilStateOff->Release();
	if (m_DepthStencilBuffer) m_DepthStencilBuffer->Release();
	if (m_RenderTargetView) m_RenderTargetView->Release();
	if (m_DeviceContext) m_DeviceContext->Release();
	if (m_Device) m_Device->Release();
	if (m_SwapChain) m_SwapChain->Release();
}
