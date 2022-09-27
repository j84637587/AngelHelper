#include "Controller.h"

Controller::Controller(HMODULE hModule) {
    this->hModule = hModule;

    //InitUI();

    this->moduleBase = (uintptr_t)GetModuleHandle(L"bao.dat");
    this->SayFunc = (_SayFunc)(moduleBase + 0x001C49D0);
    this->SkillFunc = (_SkillFunc)(moduleBase + 0x001C43A0);
    this->CallToSendFunc = (_CallToSendFunc)(moduleBase + 0x007A5A10);
    this->DoUseItemFunc = (_DoUseItemFunc)(moduleBase + 0x001CCC90);


    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "DLL Injected!\n";
}

void Controller::InitUI() {
    // Create application window
    ImGui_ImplWin32_EnableDpiAwareness();
    this->wc = { sizeof(WNDCLASSEX), CS_CLASSDC, UI::WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("ImGui Standalone"), nullptr };
    ::RegisterClassEx(&(this->wc));
    this->hwnd = ::CreateWindow(wc.lpszClassName, _T("MS Shield"), WS_OVERLAPPEDWINDOW, 100, 100, 50, 50, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!UI::CreateDeviceD3D(hwnd))
    {
        UI::CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_HIDE);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 設定中文字型
    io = ImGui::GetIO(); (void)io;
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msjh.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    IM_ASSERT(font != NULL);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 4.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(UI::pd3dDevice, UI::pd3dDeviceContext);
}

DWORD WINAPI Controller::Logic() {
    while (true)
    {
        //Render();
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            SayFunc("hello");
            std::cout << "SayFunc\n";
        }
        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {
            //SkillFunc(2, 0x1C19A610);
            DoUseItemFunc(0x16, 0x0, 0x0);
            std::cout << "DoUseItemFunc\n";
        }
        Sleep(10);
    }
    Close();
}

void Controller::Render() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    Drawing::ShowWindow(); // 主視窗
    ImGui::EndFrame();

    ImGui::Render();
    UI::pd3dDeviceContext->OMSetRenderTargets(1, &UI::pMainRenderTargetView, nullptr);
    UI::pd3dDeviceContext->ClearRenderTargetView(UI::pMainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    UI::pSwapChain->Present(1, 0);
}

void Controller::Close() {
    //ClearUI();
    fclose(this->f);
    FreeConsole();
    FreeLibraryAndExitThread(this->hModule, 0);
}

void Controller::ClearUI() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    UI::CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}


DWORD WINAPI MainThread(HMODULE hModule) {
    Controller* controller = new Controller(hModule);

    controller->Logic();

    return TRUE;
}

