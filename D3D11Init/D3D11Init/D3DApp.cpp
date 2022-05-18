#include "D3DApp.h"

bool D3DApp::CreateApp(HWND hwnd)
{
    if (!CreateDevice())
        return false;
    if (!CreateSwapChaine(hwnd))
        return false;
    if (!RenderTargetView())
        return false;
    if (!DepthStencill())
        return false;


    SetViewPort();
    return true;
}

void D3DApp::Render()
{
    float ClearColor[4] = { 0.0f, 0.125f , 0.3f , 1.0f };
    m_D3dImmdiateContext->ClearRenderTargetView(m_RenderTargetView , ClearColor);
    m_SwapChain->Present(0,0);
}

bool D3DApp::CreateDevice()
{
    D3D_FEATURE_LEVEL featureLevel[4] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    UINT createDeviceFlags = 0;

#if (defined(DEBUG) || defined(_DEBUG))
    createDeviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif

    auto result = D3D11CreateDevice(nullptr , D3D_DRIVER_TYPE_HARDWARE ,
        nullptr, createDeviceFlags , featureLevel,4 ,D3D11_SDK_VERSION , 
        &m_Device, &m_FeatureLevel, &m_D3dImmdiateContext);
    if (FAILED(result))
    {
        return false;
    }    

    return true;
}

bool D3DApp::CreateSwapChaine(HWND hwnd)
{

    if (m_Device == nullptr) return false;

    DXGI_MODE_DESC desc;    
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(DXGI_MODE_DESC));

    //백 버퍼 설정
    scd.BufferCount = 2;    
    scd.BufferDesc.Width = m_Width;
    scd.BufferDesc.Height = m_Height;
    scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    //Frame 정하는 부분
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;   

    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //전체 화면 할 것이지 정하는 부분입니다.
    scd.OutputWindow = hwnd;

    //멀티샘플링을 죵료 합니다.

    if (m_Enable4xMsaa)
    {
        scd.SampleDesc.Count = 4;
        scd.SampleDesc.Quality = 3;
    }
    else
    {

        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;
    }

    scd.Windowed = m_FullScreen;
    
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = 0;


    IDXGIDevice* dxgiDevice = 0;    
    m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    IDXGIAdapter* dxgiAdapter = 0;
    dxgiDevice->GetParent(__uuidof(dxgiAdapter), (void**)&dxgiAdapter);
    IDXGIFactory* dxgiFactory = 0;
    dxgiAdapter->GetParent(__uuidof(dxgiFactory), (void**)&dxgiFactory);


    if (FAILED(dxgiFactory->CreateSwapChain(m_Device, &scd, &m_SwapChain)))
        return false;
    

    dxgiFactory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    
    return true;
}


bool D3DApp::RenderTargetView()
{

    m_RenderTargetView = nullptr;    

    ID3D11Texture2D* backBuffer = nullptr;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer));

    if (FAILED(m_Device->CreateRenderTargetView(backBuffer, 0, &m_RenderTargetView)))
        return false;
    
    if (backBuffer != nullptr)
        backBuffer->Release();


    return true;
}



bool D3DApp::DepthStencill()
{
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width = m_Width;
    depthStencilDesc.Height = m_Height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    if (m_Enable4xMsaa)
    {
        depthStencilDesc.SampleDesc.Count = 4;
        depthStencilDesc.SampleDesc.Quality = 3;
    }
    else
    {

        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
    }

    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D* mDepthStencilBuffer;
    ID3D11DepthStencilView* mDepthStencilView;

    if (FAILED(m_Device->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer)))
    {
        return false;
    }

    if (FAILED(m_Device->CreateDepthStencilView(mDepthStencilBuffer , nullptr , &mDepthStencilView)))
    {
        return false;
    }
    

    //OM은 OutputManager라고 한다. 그래픽 버퍼 써놓는것을 OM이라고 합니다.
    m_D3dImmdiateContext->OMSetRenderTargets(1, &m_RenderTargetView, mDepthStencilView);   

    return true;
}
void D3DApp::SetViewPort()
{
    D3D11_VIEWPORT vp;

    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = static_cast<float>(m_Width);
    vp.Height = static_cast<float>(m_Height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    m_D3dImmdiateContext->RSSetViewports(1, &vp);    

}