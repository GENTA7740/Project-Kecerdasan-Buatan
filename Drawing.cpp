#include "Drawing.h"
#include <d3d11.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <tchar.h>
#include <FontAwesome5.h>
#include <Gothic.h>
#include <memory>
#include <ImWrap.h>
#include <ctime>

ID3D11Device*                g_pd3dDevice = nullptr;
ID3D11DeviceContext*         g_pd3dDeviceContext = nullptr;
IDXGISwapChain*              g_pSwapChain = nullptr;
bool                         g_SwapChainOccluded = false;
UINT                         g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView*      g_mainRenderTargetView = nullptr;
HWND                         g_windowHandle = nullptr;
WNDCLASSEXW                  g_windowClass;


ImVec4 clear_color = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImFont* g_GothicFont = nullptr;
ImFont* g_IconFont = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void Drawing::InitDrawing()
{
    g_windowClass = { sizeof(g_windowClass), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Project-AI", nullptr };
    ::RegisterClassExW(&g_windowClass);
    g_windowHandle = ::CreateWindowW(g_windowClass.lpszClassName, L"Project-AI", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, g_windowClass.hInstance, nullptr);

    if (!CreateDeviceD3D(g_windowHandle))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(g_windowClass.lpszClassName, g_windowClass.hInstance);
        return;
    }

    ::ShowWindow(g_windowHandle, SW_SHOWDEFAULT);
    ::UpdateWindow(g_windowHandle);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(g_windowHandle);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    this->InitImGui();
}
void Drawing::InitImGui() {
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    static const ImWchar icon_range[]{ ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig iConf;
    iConf.MergeMode = true;
    iConf.PixelSnapH = true;
    iConf.OversampleH = 3;
    iConf.OversampleV = 3;

    auto& io = ImGui::GetIO();
    g_GothicFont = io.Fonts->AddFontFromMemoryTTF((void*)gothic, sizeof(gothic), 17, &font_cfg);
    g_IconFont = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 17.f, &iConf, icon_range);

    ImGui::GentaLegacyColoring(&ImGui::GetStyle());
}
void Drawing::HandleMessage(bool & done)
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            done = true;
    }
}
void Drawing::DrawTopRightCorner()
{
    ImGuiViewport* vp = ImGui::GetMainViewport();
    static constexpr  float padding{ 10.0f };
    static constexpr  float padBetween{ 8.0f };
    static constexpr  float rounding{ 10.0f };
    static constexpr  ImVec2 winPad{ ImVec2(12,6) };
    static constexpr  ImVec2 itemSpc{ ImVec2(12,0) };
    static float userRegionWidth{ 0.0f };
    static float ghRegionWidth{ 0.0f };


    {

        ImVec2 pos = ImVec2(vp->WorkPos.x + vp->WorkSize.x - padding,
            vp->WorkPos.y + padding);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.85f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, winPad);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, itemSpc);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, rounding);

        if (ImGui::Begin("##UserInfoPanel", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav))
        {
            ImGui::AlignTextToFramePadding();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3f, 0.3f, 1));
            ImGui::Text("%s", ICON_FA_USER);
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 4);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            ImGui::Text("Kelompok 1");
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3f, 0.3f, 1));
            ImGui::Text("%s", ICON_FA_CHART_LINE);
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 4);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            ImGui::Text("%d fps", (int)ImGui::GetIO().Framerate);
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.3f, 0.3f, 1));
            ImGui::Text("%s", ICON_FA_CLOCK);
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 4);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
            time_t now = std::time(nullptr);
            ImGui::Text("%02d:%02d", std::localtime(&now)->tm_hour, localtime(&now)->tm_min);

            ImGui::PopStyleColor();

            userRegionWidth = ImGui::GetWindowSize().x;
        }
        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    {
        ImVec2 pos = ImVec2(vp->WorkPos.x + vp->WorkSize.x
            - padding
            - userRegionWidth
            - padBetween,
            vp->WorkPos.y + padding);
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.85f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, winPad);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

        if (ImGui::Begin("##RegionLabel", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::rbw);
            if (ImGui::Button("GH"))
            {
                ImGui::m_DrawSettingWindow = !ImGui::m_DrawSettingWindow;
            }
            ImGui::PopStyleColor(4);

            ghRegionWidth = ImGui::GetWindowSize().x;
        }
        ImGui::End();

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(3);
    }
}
void Drawing::RenderHook()
{

    ImGui::RGBEvent(); 
    ImGui::DynamicVectorGeometryLines();
    this->DrawTopRightCorner();
    this->OnSettingWindow();
}
void Drawing::OnSettingWindow()
{
    if (ImGui::m_DrawSettingWindow) {
        ImGui::Begin("Setting");
        {
            ImGui::Checkbox("Draw Gradient", &ImGui::m_DrawGradient);
            if (ImGui::m_DrawGradient) {
                ImGui::ColorEdit4("Top Color", (float*)&ImGui::m_ColorGradient[0]);
                ImGui::ColorEdit4("Mid Color 1", (float*)&ImGui::m_ColorGradient[1]);
                ImGui::ColorEdit4("Mid Color 2", (float*)&ImGui::m_ColorGradient[2]);
                ImGui::ColorEdit4("Bottom Color", (float*)&ImGui::m_ColorGradient[3]);
            }

            if (ImGui::ColorEdit4("Accent Color", (float*)&ImGui::m_ColorAccent)) {
                auto & style = ImGui::GetStyle();
                style.Colors[ImGuiCol_::ImGuiCol_Border] = ImGui::m_ColorAccent;
                style.Colors[ImGuiCol_::ImGuiCol_Separator] = ImGui::m_ColorAccent;
                style.Colors[ImGuiCol_::ImGuiCol_ScrollbarGrab] = ImGui::m_ColorAccent;
                style.Colors[ImGuiCol_::ImGuiCol_CheckMark] = ImGui::m_ColorAccent;
                style.Colors[ImGuiCol_::ImGuiCol_Header] = ImGui::m_ColorAccent;
                style.Colors[ImGuiCol_::ImGuiCol_ButtonHovered] = ImGui::m_ColorAccent;
            }
            ImGui::SliderFloat("Animation Speed", &ImGui::m_AnimSpeed, 0.0f, 10.0f, "%.2f");
            ImGui::SliderInt("Max Lines", &ImGui::m_maxLines, 0, 500);
            ImGui::SliderFloat("Max Distance", &ImGui::m_maxDistance, 0.0f, 1000.0f, "%.1f");
            ImGui::SliderInt("Dots Count", &ImGui::m_dotsCount, 0, 1000);
            ImGui::SliderFloat("Circle Size", &ImGui::m_circleSize, 0.0f, 100.0f, "%.1f");
            ImGui::SliderFloat("Move Delay", &ImGui::m_moveDelay, 0.0f, 5.0f, "%.3f");
            ImGui::SliderFloat("Blink Speed", &ImGui::m_blinkSpeed, 0.0f, 10.0f, "%.2f");
        }
        ImGui::End();
    }
}
void Drawing::OnEndFrame()
{
    ImGui::Render();
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    HRESULT hr = g_pSwapChain->Present(1, 0);
    g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}
void Drawing::OnStartFrame()
{
    if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
    {
        ::Sleep(10);
    }
    g_SwapChainOccluded = false;

    if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
    {
        this->CleanupRenderTarget();
        g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
        g_ResizeWidth = g_ResizeHeight = 0;
        this->CreateRenderTarget();
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();
}
void Drawing::OnCleanUp()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    this->CleanupDeviceD3D();
    ::DestroyWindow(g_windowHandle);
    ::UnregisterClassW(g_windowClass.lpszClassName, g_windowClass.hInstance);
}
bool Drawing::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    this->CreateRenderTarget();
    return true;
}

void Drawing::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void Drawing::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void Drawing::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}
