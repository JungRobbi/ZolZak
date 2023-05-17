#include "stdafx.h"
#include "UILayer.h"
#include <iostream>
#include <wchar.h>

using namespace std;

UILayer::UILayer(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight)
{
    m_fWidth = static_cast<float>(nWidth);
    m_fHeight = static_cast<float>(nHeight);
    m_nRenderTargets = nFrames;
    m_ppd3d11WrappedRenderTargets = new ID3D11Resource*[nFrames];
    m_ppd2dRenderTargets = new ID2D1Bitmap1*[nFrames];

    m_nTextBlocks = nTextBlocks;
    m_pTextBlocks = new TextBlock[nTextBlocks];

    InitializeDevice(pd3dDevice, pd3dCommandQueue, ppd3dRenderTargets);
}

void UILayer::InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets)
{
    UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = { };

#if defined(_DEBUG) || defined(DBG)
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ID3D11Device* pd3d11Device = NULL;
    ID3D12CommandQueue* ppd3dCommandQueues[] = { pd3dCommandQueue };
    ::D3D11On12CreateDevice(pd3dDevice, d3d11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown**>(ppd3dCommandQueues), _countof(ppd3dCommandQueues), 0, (ID3D11Device **)&pd3d11Device, (ID3D11DeviceContext **)&m_pd3d11DeviceContext, nullptr);

    pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void **)&m_pd3d11On12Device);
    pd3d11Device->Release();

#if defined(_DEBUG) || defined(DBG)
    ID3D12InfoQueue* pd3dInfoQueue;
    if (SUCCEEDED(pd3dDevice->QueryInterface(IID_PPV_ARGS(&pd3dInfoQueue))))
    {
        D3D12_MESSAGE_SEVERITY pd3dSeverities[] = { D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_MESSAGE_ID pd3dDenyIds[] = { D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE };

        D3D12_INFO_QUEUE_FILTER d3dInforQueueFilter = { };
        d3dInforQueueFilter.DenyList.NumSeverities = _countof(pd3dSeverities);
        d3dInforQueueFilter.DenyList.pSeverityList = pd3dSeverities;
        d3dInforQueueFilter.DenyList.NumIDs = _countof(pd3dDenyIds);
        d3dInforQueueFilter.DenyList.pIDList = pd3dDenyIds;

        pd3dInfoQueue->PushStorageFilter(&d3dInforQueueFilter);
    }
    pd3dInfoQueue->Release();
#endif

    IDXGIDevice* pdxgiDevice = NULL;
    m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pdxgiDevice);

    ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void **)&m_pd2dFactory);
    HRESULT hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, (ID2D1Device2 **)&m_pd2dDevice);
    m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, (ID2D1DeviceContext2 **)&m_pd2dDeviceContext);

    m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&m_pd2dWriteFactory);
    pdxgiDevice->Release();

    D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
        m_pd3d11On12Device->CreateWrappedResource(ppd3dRenderTargets[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_ppd3d11WrappedRenderTargets[i]));
        IDXGISurface* pdxgiSurface = NULL;
        m_ppd3d11WrappedRenderTargets[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
        m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
        pdxgiSurface->Release();
    }
}

ID2D1SolidColorBrush* UILayer::CreateBrush(D2D1::ColorF d2dColor)
{
    ID2D1SolidColorBrush* pd2dDefaultTextBrush = NULL;
    m_pd2dDeviceContext->CreateSolidColorBrush(d2dColor, &pd2dDefaultTextBrush);

    return(pd2dDefaultTextBrush);
}

IDWriteTextFormat* UILayer::CreateTextFormat(WCHAR* pszFontName, float fFontSize)
{
    IDWriteTextFormat* pdwDefaultTextFormat = NULL;
    m_pd2dWriteFactory->CreateTextFormat(L"맑은 고딕", nullptr, 
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        fFontSize, L"en-us", &pdwDefaultTextFormat);
    
  //pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED); // 좌측 정렬
    pdwDefaultTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); // 가운데 정렬
    pdwDefaultTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    //m_pd2dWriteFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fSmallFontSize, L"en-us", &m_pdwDefaultTextFormat);

    return(pdwDefaultTextFormat);
}

void UILayer::UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush)
{
    if (pstrUIText) wcscpy_s(m_pTextBlocks[nIndex].m_pstrText, 256, pstrUIText);
    if (pd2dLayoutRect) m_pTextBlocks[nIndex].m_d2dLayoutRect = *pd2dLayoutRect;
    if (pdwFormat) m_pTextBlocks[nIndex].m_pdwFormat = pdwFormat;
    if (pd2dTextBrush) m_pTextBlocks[nIndex].m_pd2dTextBrush = pd2dTextBrush;
}

void UILayer::Render(UINT nFrame)
{
    ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[nFrame] };

    m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[nFrame]);
    m_pd3d11On12Device->AcquireWrappedResources(ppResources, _countof(ppResources));

    m_pd2dDeviceContext->BeginDraw();
    for (UINT i = 0; i < m_nTextBlocks; i++)
    {
        m_pd2dDeviceContext->DrawText(m_pTextBlocks[i].m_pstrText, (UINT)wcslen(m_pTextBlocks[i].m_pstrText), m_pTextBlocks[i].m_pdwFormat, m_pTextBlocks[i].m_d2dLayoutRect, m_pTextBlocks[i].m_pd2dTextBrush);
    }
    m_pd2dDeviceContext->EndDraw();

    m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    m_pd3d11DeviceContext->Flush();
}

void UILayer::ReleaseResources()
{
    for (UINT i = 0; i < m_nTextBlocks; i++)
    {
        m_pTextBlocks[i].m_pdwFormat->Release();
        m_pTextBlocks[i].m_pd2dTextBrush->Release();
    }

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        ID3D11Resource* ppResources[] = { m_ppd3d11WrappedRenderTargets[i] };
        m_pd3d11On12Device->ReleaseWrappedResources(ppResources, _countof(ppResources));
    }

    m_pd2dDeviceContext->SetTarget(nullptr);
    m_pd3d11DeviceContext->Flush();

    for (UINT i = 0; i < m_nRenderTargets; i++)
    {
        m_ppd2dRenderTargets[i]->Release();
        m_ppd3d11WrappedRenderTargets[i]->Release();
    }

    m_pd2dDeviceContext->Release();
    m_pd2dWriteFactory->Release();
    m_pd2dDevice->Release();
    m_pd2dFactory->Release();
    m_pd3d11DeviceContext->Release();
    m_pd3d11On12Device->Release();
}

///////////////
//////////////
////////////////

E_MODE_CHAT ChatMGR::m_ChatMode{ E_MODE_CHAT::E_MODE_PLAY };
E_MODE_HANGUL ChatMGR::m_HangulMode{ E_MODE_HANGUL::E_MODE_ENGLISH };
UILayer* ChatMGR::m_pUILayer = nullptr;
WCHAR ChatMGR::m_textbuf[256];
WCHAR ChatMGR::m_combtext;
int ChatMGR::m_textindex{ 0 };

std::list<WCHAR*> ChatMGR::m_pPrevTexts;

ID2D1SolidColorBrush* ChatMGR::pd2dBrush;
IDWriteTextFormat* ChatMGR::pdwTextFormat;
D2D1_RECT_F ChatMGR::d2dRect;

void ChatMGR::UpdateText()
{
    m_pUILayer->UpdateTextOutputs(0, m_textbuf, &d2dRect, pdwTextFormat, pd2dBrush);
    m_pUILayer->UpdateTextOutputs(1, m_textbuf, &d2dRect, pdwTextFormat, pd2dBrush);
}

void ChatMGR::SetTextinfos(int WndClientWidth, int WndClientHeight)
{
    memset(m_textbuf, NULL, sizeof(m_textbuf));
    m_combtext = NULL;

    pd2dBrush = m_pUILayer->CreateBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
    pdwTextFormat = m_pUILayer->CreateTextFormat(L"Arial", WndClientHeight / 25.0f);
  //d2dRect = D2D1::RectF((float)WndClientWidth / 3.2f, (float)WndClientHeight / 1.83f, (float)WndClientWidth, (float)WndClientHeight); // 좌측 정렬
    d2dRect = D2D1::RectF(0, (float)WndClientHeight / 1.83f, (float)WndClientWidth, (float)WndClientHeight); // 가운데 정렬
    SetInGame(WndClientWidth, WndClientHeight);
}

void ChatMGR::SetTextSort(int WndClientWidth, int WndClientHeight, E_CHAT_SORTTYPE type)
{
    if (type == E_CHAT_SORTTYPE::E_SORTTYPE_LEFT) {
        pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED); // 좌측 정렬
    }
    else if (type == E_CHAT_SORTTYPE::E_SORTTYPE_MID) {
        pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); // 가운데 정렬
    }
}

void ChatMGR::StoreText()
{
    WCHAR temp[256];
    wcscpy_s(temp, m_textbuf);
    m_pPrevTexts.push_back(temp);
    if (m_pPrevTexts.size() >= 10) {
        m_pPrevTexts.pop_front();
    }
}

void ChatMGR::SetLoginScene(int WndClientWidth, int WndClientHeight)
{
    memset(m_textbuf, NULL, sizeof(m_textbuf));
    m_combtext = NULL;

    SetTextSort(WndClientWidth, WndClientHeight, E_CHAT_SORTTYPE::E_SORTTYPE_MID);
    d2dRect = D2D1::RectF(0, (float)WndClientHeight / 1.83f, (float)WndClientWidth, (float)WndClientHeight); // 가운데 정렬
}

void ChatMGR::SetInGame(int WndClientWidth, int WndClientHeight)
{
    memset(m_textbuf, NULL, sizeof(m_textbuf));
    m_combtext = NULL;

    SetTextSort(WndClientWidth, WndClientHeight, E_CHAT_SORTTYPE::E_SORTTYPE_LEFT);
    d2dRect = D2D1::RectF((float)WndClientWidth / 48.f, (float)WndClientHeight / 1.4f, (float)WndClientWidth, (float)WndClientHeight); // 좌측 정렬

    m_pUILayer->m_pd2dWriteFactory->CreateTextFormat(L"맑은 고딕", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        WndClientHeight / 50.0f, L"en-us", &pdwTextFormat);

}
