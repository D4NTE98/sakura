#include "../libs/xor/xorstr.h"
#include "../../imgui/imgui.h"

#include "menu.h"
#include "injector.h"

#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

bool Sakura::Menu::Active = true;

ImGuiWindowFlags Sakura::Menu::WindowFlags =
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoMove;

bool stylesInit = false;
bool autoClose = false;
bool autoInject = false;

char modulePath[MAX_PATH];
char moduleName[32];
char statusText[128];

PROCESS_INFORMATION processInfo;

static ULONGLONG lastAutoInjectAttempt = 0;

enum class LoaderStatus
{
    Ready,
    Error,
    Success
};

LoaderStatus loaderStatus = LoaderStatus::Ready;

void Sakura::Init()
{
    sprintf_s(statusText, sizeof(statusText), "Ready to inject");
    sprintf_s(moduleName, sizeof(moduleName), "sakura.dll");
}

void Sakura::InitStyles()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(0.0f, 0.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(10.0f, 10.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;

    style.WindowRounding = 13.0f;
    style.FrameRounding = 8.0f;
    style.ChildRounding = 10.0f;
    style.GrabRounding = 8.0f;

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.035f, 0.035f, 0.043f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.065f, 0.065f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.94f, 0.94f, 0.96f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.56f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.14f, 0.17f, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(0.78f, 0.19f, 0.43f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.88f, 0.24f, 0.49f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.68f, 0.15f, 0.36f, 1.0f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.13f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.23f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.38f, 0.58f, 1.0f);

    stylesInit = true;
}

void Sakura::Inject()
{
    bool moduleExists = true;
    bool processExists = true;

    GetModuleFileNameA(nullptr, modulePath, MAX_PATH);

    char* index = strrchr(modulePath, '\\');

    if (index)
        modulePath[index - modulePath + 1] = 0;

    strcat_s(modulePath, MAX_PATH, moduleName);

    WIN32_FIND_DATAA fileData;
    HANDLE fileHandle = FindFirstFileA(modulePath, &fileData);

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        sprintf_s(statusText, sizeof(statusText), "sakura.dll was not found");
        loaderStatus = LoaderStatus::Error;
        moduleExists = false;
    }
    else
    {
        FindClose(fileHandle);
    }

    if (!moduleExists)
        return;

    if (!Injector::GetProcessInfo("hl.exe", &processInfo))
    {
        sprintf_s(statusText, sizeof(statusText), "Start Counter-Strike 1.6 first");
        loaderStatus = LoaderStatus::Error;
        processExists = false;
    }

    if (!processExists)
        return;

    if (Injector::InjectModule("hl.exe", modulePath))
    {
        sprintf_s(statusText, sizeof(statusText), "Sakura loaded successfully");
        loaderStatus = LoaderStatus::Success;

        if (autoClose)
            exit(0);
    }
    else
    {
        sprintf_s(statusText, sizeof(statusText), "Injection failed");
        loaderStatus = LoaderStatus::Error;
    }
}

void Sakura::Menu::Draw()
{
    if (!Active)
        return;

    if (!stylesInit)
        InitStyles();

    if (autoInject && loaderStatus != LoaderStatus::Success)
    {
        const ULONGLONG currentTime = GetTickCount64();

        if (currentTime - lastAutoInjectAttempt >= 1000)
        {
            lastAutoInjectAttempt = currentTime;
            Sakura::Inject();
        }
    }

    const ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin("##sakura_loader", &Active, WindowFlags);

    const ImVec2 windowPos = ImGui::GetWindowPos();
    const ImVec2 windowSize = ImGui::GetWindowSize();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    constexpr float titleBarHeight = 52.0f;
    constexpr float contentPadding = 28.0f;
    constexpr float controlButtonSize = 32.0f;
    constexpr float statusTop = 112.0f;
    constexpr float statusHeight = 78.0f;
    constexpr float optionsTop = 204.0f;
    constexpr float optionsHeight = 118.0f;
    constexpr float actionHeight = 52.0f;
    constexpr float actionBottomMargin = 28.0f;

    drawList->AddRectFilled(
        windowPos,
        ImVec2(windowPos.x + windowSize.x, windowPos.y + titleBarHeight),
        IM_COL32(17, 17, 22, 255),
        13.0f,
        ImDrawFlags_RoundCornersTop
    );

    drawList->AddCircleFilled(
        ImVec2(windowPos.x + 22.0f, windowPos.y + 26.0f),
        5.0f,
        IM_COL32(235, 69, 128, 255)
    );

    ImGui::SetCursorPos(ImVec2(36.0f, 17.0f));
    ImGui::TextColored(ImVec4(0.92f, 0.27f, 0.50f, 1.0f), "SAKURA");
    ImGui::SameLine(0.0f, 9.0f);
    ImGui::TextDisabled("Loader 1.1");

    ImGui::SetCursorPos(
        ImVec2(
            windowSize.x - (controlButtonSize * 2.0f) - 18.0f,
            10.0f
        )
    );

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.16f, 0.20f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.12f, 0.12f, 0.15f, 1.0f));

    if (ImGui::Button("-##minimize", ImVec2(controlButtonSize, controlButtonSize)))
    {
        HWND hwnd = ::GetActiveWindow();

        if (hwnd)
            ::ShowWindow(hwnd, SW_MINIMIZE);
    }

    ImGui::SameLine(0.0f, 4.0f);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.78f, 0.18f, 0.28f, 1.0f));

    if (ImGui::Button("X##close", ImVec2(controlButtonSize, controlButtonSize)))
        ::PostQuitMessage(0);

    ImGui::PopStyleColor();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::SetCursorPos(ImVec2(contentPadding, 78.0f));
    ImGui::Text("Counter-Strike 1.6");
    ImGui::SameLine(0.0f, 8.0f);
    ImGui::TextDisabled("/ DLL loader");

    const float contentWidth = windowSize.x - (contentPadding * 2.0f);

    ImGui::SetCursorPos(ImVec2(contentPadding, statusTop));
    ImGui::BeginChild(
        "##status_box",
        ImVec2(contentWidth, statusHeight),
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    ImGui::SetCursorPos(ImVec2(18.0f, 14.0f));
    ImGui::TextDisabled("STATUS");

    ImVec4 statusColor = ImVec4(0.80f, 0.80f, 0.84f, 1.0f);
    const char* statusBadge = "READY";

    if (loaderStatus == LoaderStatus::Success)
    {
        statusColor = ImVec4(0.36f, 0.82f, 0.48f, 1.0f);
        statusBadge = "LOADED";
    }
    else if (loaderStatus == LoaderStatus::Error)
    {
        statusColor = ImVec4(0.95f, 0.32f, 0.32f, 1.0f);
        statusBadge = "ERROR";
    }

    ImGui::SetCursorPos(ImVec2(18.0f, 43.0f));
    ImGui::TextColored(statusColor, "%s", statusText);

    const ImVec2 badgeTextSize = ImGui::CalcTextSize(statusBadge);
    const float badgeWidth = badgeTextSize.x + 24.0f;
    const float badgeHeight = 30.0f;
    const float badgeX = contentWidth - badgeWidth - 16.0f;
    const float badgeY = 25.0f;

    const ImVec2 childScreenPos = ImGui::GetWindowPos();
    ImDrawList* statusDrawList = ImGui::GetWindowDrawList();

    statusDrawList->AddRectFilled(
        ImVec2(childScreenPos.x + badgeX, childScreenPos.y + badgeY),
        ImVec2(childScreenPos.x + badgeX + badgeWidth, childScreenPos.y + badgeY + badgeHeight),
        ImGui::GetColorU32(ImVec4(statusColor.x, statusColor.y, statusColor.z, 0.14f)),
        8.0f
    );

    statusDrawList->AddText(
        ImVec2(
            childScreenPos.x + badgeX + (badgeWidth - badgeTextSize.x) * 0.5f,
            childScreenPos.y + badgeY + (badgeHeight - badgeTextSize.y) * 0.5f
        ),
        ImGui::GetColorU32(statusColor),
        statusBadge
    );

    ImGui::EndChild();

    ImGui::SetCursorPos(ImVec2(contentPadding, optionsTop));
    ImGui::BeginChild(
        "##options_box",
        ImVec2(contentWidth, optionsHeight),
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    ImGui::SetCursorPos(ImVec2(18.0f, 14.0f));
    ImGui::TextDisabled("OPTIONS");

    ImGui::SetCursorPos(ImVec2(18.0f, 43.0f));
    ImGui::Checkbox("Close loader after injection", &autoClose);

    ImGui::SetCursorPos(ImVec2(18.0f, 78.0f));
    ImGui::Checkbox("Inject automatically when the game starts", &autoInject);

    ImGui::EndChild();

    const float actionY = windowSize.y - actionBottomMargin - actionHeight;

    ImGui::SetCursorPos(ImVec2(contentPadding, actionY));

    if (loaderStatus == LoaderStatus::Success)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.42f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.42f, 0.25f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.18f, 0.42f, 0.25f, 1.0f));
        ImGui::Button("SAKURA LOADED", ImVec2(contentWidth, actionHeight));
        ImGui::PopStyleColor(3);
    }
    else if (autoInject)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f, 0.14f, 0.17f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.14f, 0.14f, 0.17f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.14f, 0.14f, 0.17f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.62f, 0.62f, 0.68f, 1.0f));
        ImGui::Button("WAITING FOR HL.EXE...", ImVec2(contentWidth, actionHeight));
        ImGui::PopStyleColor(4);
    }
    else
    {
        if (ImGui::Button("INJECT SAKURA", ImVec2(contentWidth, actionHeight)))
            Sakura::Inject();
    }

    ImGui::End();
}
