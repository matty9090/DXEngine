#pragma once

#include <string>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <D3d9types.h>

#include <vector>

#include "FW1FontWrapper.h"
#include "Mirror.hpp"

class Window;

class Graphics {
	public:
		Graphics(HWND hWnd, size_t windowW, size_t windowH);
		~Graphics();

		bool init();

		void beginScene();
		void endScene();

		void toggleWireframe();
		void createMirror(Mirror &mirror);
		void setRenderTargets(ID3D11RenderTargetView *target, ID3D11DepthStencilView *depth, D3D11_VIEWPORT &viewport);

		void getProjMatrix(D3DXMATRIX &m) { m = m_ProjectionMatrix; }
		void getOrthoMatrix(D3DXMATRIX &m) { m = m_OrthoMatrix; }

		void drawText(float x, float y, UINT32 colour, std::string str);
		void drawTextValue(float x, float y, std::string desc, float value);
		void drawTextValue(float x, float y, std::string desc, D3DXVECTOR3 value);

		D3DXMATRIX &getProjectionMatrix() { return m_ProjectionMatrix; }
		ID3D11Device *getDevice() { return m_Device; }
		ID3D11DeviceContext *getDeviceContext() { return m_DeviceContext; }

		ID3D11DepthStencilState *getDepthStateOn() { return m_DepthStencilStateOn; }
		ID3D11DepthStencilState *getDepthStateOff() { return m_DepthStencilStateOff; }

	private:
		HWND m_Hwnd;

		size_t m_WindowW, m_WindowH;
		size_t m_ViewportW, m_ViewportH;

		bool m_vsync_enabled;
		bool m_Wireframe;
		int m_videoCardMemory;
		char m_videoCardDescription[128];
		float m_Near, m_Far;

		D3D11_VIEWPORT m_Viewport;
		IFW1FontWrapper *m_FontWrapper;
		IDXGISwapChain *m_SwapChain;
		ID3D11Device *m_Device;
		ID3D11DeviceContext *m_DeviceContext;
		ID3D11RenderTargetView *m_RenderTargetView;
		ID3D11Texture2D *m_DepthStencilBuffer;
		ID3D11DepthStencilState *m_DepthStencilStateOn, *m_DepthStencilStateOff;
		ID3D11DepthStencilView *m_DepthStencilView;
		ID3D11RasterizerState *m_RasterState;

		D3DXMATRIX m_ProjectionMatrix, m_OrthoMatrix;

		bool initAdapter();
		bool initDevice();
		bool initDepthBuffer();
		bool initBackBuffer();
		bool initRaster(bool wireframe = false);
		bool initViewport();
		bool initMatrices();
		bool initFonts();
		
		void cleanup();
};

