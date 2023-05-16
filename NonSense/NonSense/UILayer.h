#pragma once
#include <string>
#include <list>
#include <memory>

#include <d2d1.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d2d1_3.h>
#include <D3Dcompiler.h>
#include <d3d11on12.h>

#include <dwrite.h>
#include <d3d11on12.h>

#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")

struct TextBlock
{
    WCHAR                           m_pstrText[256];
    D2D1_RECT_F                     m_d2dLayoutRect;
    IDWriteTextFormat*              m_pdwFormat;
    ID2D1SolidColorBrush*           m_pd2dTextBrush;
};

class UILayer
{
public:
    UILayer(UINT nFrames, UINT nTextBlocks, ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets, UINT nWidth, UINT nHeight);

    void UpdateTextOutputs(UINT nIndex, WCHAR* pstrUIText, D2D1_RECT_F* pd2dLayoutRect, IDWriteTextFormat* pdwFormat, ID2D1SolidColorBrush* pd2dTextBrush);
    void Render(UINT nFrame);
    void ReleaseResources();

    ID2D1SolidColorBrush* CreateBrush(D2D1::ColorF d2dColor);
    IDWriteTextFormat* CreateTextFormat(WCHAR* pszFontName, float fFontSize);

public:
    void InitializeDevice(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Resource** ppd3dRenderTargets);

    float                           m_fWidth = 0.0f;
    float                           m_fHeight = 0.0f;

    ID3D11DeviceContext*            m_pd3d11DeviceContext = NULL;
    ID3D11On12Device*               m_pd3d11On12Device = NULL;
    IDWriteFactory*                 m_pd2dWriteFactory = NULL;
    ID2D1Factory3*                  m_pd2dFactory = NULL;
    ID2D1Device2*                   m_pd2dDevice = NULL;
    ID2D1DeviceContext2*            m_pd2dDeviceContext = NULL;

    UINT                            m_nRenderTargets = 0;
    ID3D11Resource**                m_ppd3d11WrappedRenderTargets = NULL;
    ID2D1Bitmap1**                  m_ppd2dRenderTargets = NULL;

    UINT                            m_nTextBlocks = 0;
    TextBlock*                      m_pTextBlocks = NULL;
};

///////////////////
//////////////////
///////////////////

static char* ConvertWCtoC(wchar_t* str)
{
    char* pStr;
    int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    pStr = new char[strSize];
    WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
    return pStr;
}

static wchar_t* ConverCtoWC(char* str)
{
    wchar_t* pStr;
    int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
    pStr = new WCHAR[strSize];
    MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

    return pStr;

}



////////////////////
////////////////////
////////////////////

enum class E_MODE_CHAT {
    E_MODE_PLAY, E_MODE_CHAT
};

enum class E_MODE_HANGUL {
    E_MODE_ENGLISH, E_MODE_HANGUL
};

enum class E_CHAT_SORTTYPE {
    E_SORTTYPE_LEFT, E_SORTTYPE_MID
};

class ChatMGR
{
public:
    static WCHAR m_textbuf[256];
    static int m_textindex;
    static WCHAR m_combtext;

    static UILayer* m_pUILayer;
    static E_MODE_CHAT m_ChatMode;
    static E_MODE_HANGUL m_HangulMode;

    static std::list<WCHAR*> m_pPrevTexts;

    static ID2D1SolidColorBrush* pd2dBrush;
    static IDWriteTextFormat* pdwTextFormat;
    static D2D1_RECT_F d2dRect;

    static void SetTextinfos(int WndClientWidth, int WndClientHeight);
    static void SetTextSort(int WndClientWidth, int WndClientHeight, E_CHAT_SORTTYPE type);
    static void UpdateText();
    static void StoreText();
};
