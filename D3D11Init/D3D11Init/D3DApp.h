#pragma once
#include "Common.h"

class D3DApp
{
public:
	D3DApp(int width , int height , HWND hwnd) {

		m_Width = width;
		m_Height = height;
		CreateApp(hwnd);
	}
	~D3DApp() {
			
		if (m_D3dImmdiateContext != nullptr)
			m_D3dImmdiateContext->Release();

		if (m_SwapChain != nullptr)
			m_SwapChain->Release();

		if (m_Device != nullptr)
			m_Device->Release();

		if (m_RenderTargetView != nullptr)
			m_RenderTargetView->Release();


	}

	bool CreateApp(HWND hwnd);
	void Render();

	ID3D11Device* GetDevice() { return m_Device;	}
	ID3D11DeviceContext* GetDeviceContext() { return m_D3dImmdiateContext; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetView; }

private:
	bool CreateDevice();
	bool CreateSwapChaine(HWND hwnd);
	bool RenderTargetView();
	bool DepthStencill();
	void SetViewPort();

	
	ID3D11Device* m_Device;
	ID3D11DeviceContext * m_D3dImmdiateContext;
	ID3D11RenderTargetView* m_RenderTargetView;
	

	IDXGISwapChain* m_SwapChain;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	


	bool m_Enable4xMsaa = false;
	bool m_FullScreen = true;

	int m_Width;
	int m_Height;


};

