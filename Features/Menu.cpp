#include "Menu.h"
#include "../Globals/globals.h"
#include "../Features/Misc.h"
#include "../Features/Aimbot.h"
#include "../utils/imgui/imgui.h"
#include "../utils/imgui/imgui_impl_dx9.h"
#include "../utils/imgui/imgui_impl_win32.h"
#include "../Hooks/IDirect3DDevice9.h"

#include <cmath>
#include <iostream>
#include <string>

WNDPROC originalWndProc = nullptr;

LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (Menu::bShow)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		I::NInputSystem->ResetInputState();

		return true;
	}

	return CallWindowProc(originalWndProc, hWnd, uMsg, wParam, lParam);
}

ImVec4 GetRainBowColor()
{
	static float time = 0.0f;
	time += ImGui::GetIO().DeltaTime * 5.0f;

	ImVec4 rainbowColor;
	rainbowColor.x = sinf(time) * 0.5f + 0.5f;
	rainbowColor.y = sinf(time + 2.0f * M_PI / 3.0f) * 0.5f + 0.5f;
	rainbowColor.z = sinf(time + 4.0f * M_PI / 3.0f) * 0.5f + 0.5f;
	rainbowColor.w = 1.0f;

	return rainbowColor;
}

void ApplyStyles();

void Menu::Init(IDirect3DDevice9* pDevice, HWND hwnd)
{
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGui_ImplDX9_Init(pDevice);
	ImGui_ImplWin32_Init(hwnd);

    ImGuiIO& io = ImGui::GetIO();
    ImFont* verdanaFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 14.f);
    io.FontDefault = verdanaFont;

    ApplyStyles();
}

void ApplyStyles()
{

    ImGuiStyle& style = ImGui::GetStyle();
    auto& colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.04f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.05f, 0.07f, 0.60f);
    colors[ImGuiCol_Border] = ImVec4(0.05f, 0.07f, 0.09f, 0.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_Text] = ImVec4(0.88f, 0.89f, 0.90f, 0.85f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.28f, 0.32f, 0.37f, 1.00f);

    ImVec4 accentColor = ImVec4(0.12f, 0.40f, 0.78f, 0.95f);       
    ImVec4 accentColorHover = ImVec4(0.18f, 0.50f, 0.88f, 1.00f);
    ImVec4 accentColorActive = ImVec4(0.08f, 0.33f, 0.72f, 1.00f);
    ImVec4 accentColorDim = ImVec4(0.10f, 0.20f, 0.36f, 0.54f);

    colors[ImGuiCol_Button] = accentColorDim;
    colors[ImGuiCol_ButtonHovered] = accentColorHover;
    colors[ImGuiCol_ButtonActive] = accentColorActive;

    colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.13f, 0.15f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.10f, 0.12f, 0.14f, 0.67f);

    colors[ImGuiCol_SliderGrab] = accentColorDim;
    colors[ImGuiCol_SliderGrabActive] = accentColorActive;
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.26f, 0.51f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.33f, 0.34f, 0.51f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.42f, 0.90f);

    colors[ImGuiCol_Header] = ImVec4(0.12f, 0.15f, 0.20f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = accentColorDim;
    colors[ImGuiCol_HeaderActive] = accentColor;
    colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.10f, 0.15f, 0.86f);
    colors[ImGuiCol_TabHovered] = accentColorHover;
    colors[ImGuiCol_TabActive] = accentColor;
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.10f, 0.15f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.10f, 0.18f, 0.32f, 0.66f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.08f, 0.10f, 0.80f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.14f, 0.80f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.06f, 0.08f, 0.80f);

    colors[ImGuiCol_CheckMark] = ImVec4(0.18f, 0.60f, 0.90f, 1.00f);

    colors[ImGuiCol_Separator] = ImVec4(0.12f, 0.25f, 0.40f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.18f, 0.40f, 0.68f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.18f, 0.40f, 0.68f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.40f, 0.68f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.18f, 0.40f, 0.68f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.18f, 0.40f, 0.68f, 0.95f);

    colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.04f, 0.05f, 0.94f);

    colors[ImGuiCol_PlotLines] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.80f, 0.30f, 0.25f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.70f, 0.55f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.80f, 0.50f, 0.00f, 1.00f);

    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 4.0f;

    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(8, 6);
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.TouchExtraPadding = ImVec2(0, 0);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 8.0f;
    style.GrabMinSize = 5.0f;

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6f;

    style.AntiAliasedLines = true;
    style.AntiAliasedLinesUseTex = true;
    style.AntiAliasedFill = true;

    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
}

void RenderAimbotTab();
void RenderTriggerBotTab();
void RenderVisualsTab();
void RenderMiscTab();
void RenderDebugTab();
void RenderColorTab();

void RenderNew()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (!Menu::bShow)
    {
        ImGui::EndFrame();
        return;
    }

    static int tabIndex = 0;

    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(1.0f);

    if (ImGui::Begin("Oh eX whY", &Menu::bShow,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::BeginChild("TopBar", ImVec2(ImGui::GetWindowWidth() - 30, 45), true, 
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        {

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
            ImGui::SetCursorPosX(20);
            ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Oh eX whY");

            /*ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "$");*/

            float width = ImGui::CalcTextSize("v2.7 | by triconquest").x;
            ImGui::SameLine(ImGui::GetWindowWidth() - width - 20);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "v2.7 | by triconquest");
        }
        ImGui::EndChild();

        ImGui::BeginChild("Sidebar", ImVec2(160, ImGui::GetContentRegionAvail().y), true);
        {
            const char* tabs[] = { "Aimbot", "TriggerBot", "Visuals", "Misc", "Debug", "Colors" };

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

            for (int i = 0; i < 6; i++) {
                ImGui::PushStyleColor(ImGuiCol_Button, (tabIndex == i) ?
                    ImVec4(0.18f, 0.60f, 0.96f, 0.4f) : ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                    ImVec4(0.18f, 0.60f, 0.96f, 0.5f));

                ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                    ImVec4(0.18f, 0.60f, 0.96f, 0.7f));

                if (tabIndex == i) {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 3);
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.18f, 0.60f, 0.96f, 1.0f));
                    ImGui::BeginChild(("SelInd" + std::to_string(i)).c_str(), ImVec2(3, 30), true);
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                    ImGui::SameLine();
                }
                else {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);
                }

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
                if(ImGui::Button(std::string(tabs[i]).c_str(), ImVec2(100, 30)))
                    tabIndex = i;

                ImGui::PopStyleColor(3);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
            }
        }
        ImGui::EndChild();

        // Main content area
        ImGui::SameLine();
        ImGui::BeginChild("Content", ImVec2(0, 0), true);
        {
            switch (tabIndex)
            {
            case 0:
                RenderAimbotTab();
                break;
            case 1:
                RenderTriggerBotTab();
                break;
            case 2:
                RenderVisualsTab();
                break;
            case 3:
                RenderMiscTab();
                break;
            case 4:
                RenderDebugTab();
                break;
            case 5:
                RenderColorTab();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void RenderAimbotTab()
{
    ImGui::BeginChild("AimbotSection", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

    ImGui::BeginChild("AimbotCol1", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, ImGui::GetContentRegionAvail().y), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Global Settings");
        ImGui::Separator();

        ImGui::Checkbox("Enable Aimbot", &G::bAimbot);

        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.18f, 0.60f, 0.96f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.25f, 0.68f, 0.99f, 1.0f));
        ImGui::PopStyleColor(2);

        ImGui::Checkbox("Silent Aim", &G::bSilentActive);

        //ImGui::Checkbox("FOV Circle", &G::bFOVCircle);
        ImGui::SliderFloat("Aim FOV", &G::fAimFOV, 0.0f, 100.f, "%1.f");

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Target Filters");
        ImGui::Separator();

        const char* hitboxes[] = { "Head", "Chest", "Pelvis"};
        int hitboxValues[] = { BONE_HEAD, BONE_CHEST, BONE_PELVIS };

        static int currentSelection = 0;
        for (int i = 0; i < IM_ARRAYSIZE(hitboxes); ++i)
        {
            if (hitboxValues[i] == G::iAimBone)
            {
                currentSelection = i;
                break;
            }
        }

        if (ImGui::Combo("Hitbox Selection", &currentSelection, hitboxes, IM_ARRAYSIZE(hitboxes)))
            G::iAimBone = hitboxValues[currentSelection];
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("AimbotCol2", ImVec2(0, 0), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Advanced Options");
        ImGui::Separator();

        ImGui::Checkbox("Auto Fire", &G::bAutoShoot);
        ImGui::Checkbox("Aim at bone", &G::bAimbotUseBones);

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Key Bindings");
        ImGui::Separator();

        const char* keys[] = { "E Key", "Always On" };
        int aimKeys[] = { AIM_E, AIM_NONE };
        static int selectedAimKey = 0;
        if (ImGui::Combo("Aim Key", &selectedAimKey, keys, IM_ARRAYSIZE(keys)))
            G::iAimHold = aimKeys[selectedAimKey];

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.92f, 0.25f, 0.2f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.3f, 0.25f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.2f, 0.18f, 1.0f));

        ImGui::PopStyleColor(3);
    }
    ImGui::EndChild();

    ImGui::EndChild();
}

void RenderTriggerBotTab()
{
    ImGui::BeginChild("TriggerBotSection", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

    // Left column container
    ImGui::BeginChild("LeftColumnContainer", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, ImGui::GetContentRegionAvail().y), false);

    // Trigger Settings section
    ImGui::BeginChild("TriggerBotCol1", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Trigger Settings");
        ImGui::Separator();
        ImGui::Checkbox("Enable TriggerBot", &G::bTriggerbot);
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Bind Settings");
        ImGui::Separator();
        static int selectedKey = 0;
        const char* keys[] = { "Always On", "E key" };
        int triggerKeys[] = { TRIGGERBOT_NONE, TRIGGERBOT_E };
        if (ImGui::Combo("Key", &selectedKey, keys, IM_ARRAYSIZE(keys)))
            G::iTriggerBotKey = triggerKeys[selectedKey];
        ImGui::Checkbox("Hold Mode", &G::bTriggerBotHoldMode);
    }
    ImGui::EndChild();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);

    // KnifeBot Settings section
    ImGui::BeginChild("KnifeBotCol", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "KnifeBot Settings");
        ImGui::Separator();
        ImGui::Checkbox("Enable KnifeBot", &G::bKnifeBot);
        ImGui::Checkbox("Backstabs Only", &G::bKnifeBotBackStabOnly);
        ImGui::Spacing();
    }
    ImGui::EndChild();

    ImGui::EndChild();

    // Right column
    ImGui::SameLine();
    ImGui::BeginChild("TriggerBotCol2", ImVec2(0, 0), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Advanced Settings");
        ImGui::Separator();
        ImGui::Checkbox("Scope Only", &G::bTriggerbot);
        //ImGui::Checkbox("Smoke Check", &G::bTriggerbot);
        ImGui::Spacing();
        ImGui::Spacing();
        /*ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Timing");
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.18f, 0.60f, 0.96f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.25f, 0.68f, 0.99f, 1.0f));
        ImGui::SliderFloat("Delay (ms)", &G::fTriggerBotDelay, 0.0f, 500.0f, "%.1f ms");
        ImGui::Spacing();*/
        ImGui::SeparatorText("Random Delays");
        ImGui::SliderFloat("Min", &G::fTriggerBotRndDelayMin, 0.0f, 0.2f, "%.3f");
        ImGui::SliderFloat("Max", &G::fTriggerBotRndDelayMax, 0.0f, 0.3f, "%.3f");
        if (G::fTriggerBotRndDelayMin > G::fTriggerBotRndDelayMax)
            ImGui::TextColored(ImVec4(255, 0, 255, 255), "Min > Max, change that.");
        else ImGui::Text("Range: (%.0f - %.0f) ms", G::fTriggerBotRndDelayMin * 1000.f, G::fTriggerBotRndDelayMax * 1000.f);

        //ImGui::PopStyleColor(2);
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Hitboxes");
        ImGui::Separator();
        ImGui::Checkbox("Custom Hitboxes", &G::bTriggerCustomHitBoxes);
        if (G::bTriggerCustomHitBoxes)
        {
            ImGui::Checkbox("Head", &G::bTriggerHead);
            ImGui::Checkbox("Chest", &G::bTriggerChest);
            ImGui::Checkbox("Stomach", &G::bTriggerStomach);
        }
    }
    ImGui::EndChild();

    ImGui::EndChild(); // End TriggerBotSection
}

void RenderVisualsTab()
{
    ImGui::BeginChild("VisualsSection", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
    // Left column
    ImGui::BeginChild("VisualsCol1", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, ImGui::GetContentRegionAvail().y), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "ESP Settings");
        ImGui::Separator();
        ImGui::Checkbox("Enable ESP", &G::bESP);
        //ImGui::Checkbox("Always On", &G::bESPAlwaysOn);
        ImGui::Checkbox("Use Bones", &G::bUseBones);
        ImGui::Checkbox("Show Team", &G::bShowTeam);
        ImGui::Checkbox("Skeleton", &G::bBoneESP);
        ImGui::Checkbox("Distance", &G::bDistanceToEnemy);
        ImGui::Checkbox("Enemy Lines", &G::bEnemyLines);
        ImGui::Checkbox("Enemies Behind", &G::bEnemiesBehind);

        //ImGui::Spacing();
        //ImGui::Spacing();
        //ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Colors");
        //ImGui::Separator();
        //static float teamColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
        //ImGui::ColorEdit3("Enemy Color", G::fColor, ImGuiColorEditFlags_NoInputs);
        //ImGui::ColorEdit3("Target Color", G::fTargetColor, ImGuiColorEditFlags_NoInputs);
        //ImGui::ColorEdit4("Team Color", teamColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }
    ImGui::EndChild();

    // Right column
    ImGui::SameLine();
    ImGui::BeginChild("VisualsCol2", ImVec2(0, 0), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "World ESP");
        ImGui::Separator();
        ImGui::Checkbox("Remove Scope", &G::bRemoveScope);
        ImGui::Checkbox("Remove Flash", &G::bNoFlash);
        //ImGui::Checkbox("Sniper Lines", &G::bEnemyScopeLines);
        //ImGui::Checkbox("Bullet Tracers", &G::bBulletShots);
        ImGui::Checkbox("Recoil Crosshair", &G::bRecoilCrosshair);
        ImGui::Checkbox("No Recoil", &G::bNoRecoil);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Misc");
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.18f, 0.60f, 0.96f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.25f, 0.68f, 0.99f, 1.0f));
        ImGui::PopStyleColor(2);

        ImGui::Checkbox("Show Enemy Weapons", &G::bShowEnemyWeapons);
        ImGui::Checkbox("Corner Box", &G::bCornerESP);
        ImGui::Checkbox("Enemy Aim", &G::bVisualEnemyAimingAtUs);
        ImGui::Checkbox("Show head bone", &G::bHeadBone);
        ImGui::Checkbox("Custom Mode Visuals", &G::bEnableCustomModeInfo);
    }
    ImGui::EndChild();
    ImGui::EndChild();
}

void RenderMiscTab()
{
    ImGui::BeginChild("MiscSection", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
    ImGui::BeginChild("MiscCol1", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, ImGui::GetContentRegionAvail().y), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Movement");
        ImGui::Separator();
        ImGui::Checkbox("Bunny Hop", &G::bBhop);
        ImGui::Checkbox("Fast Stop", &G::bFastStop);
        //ImGui::Checkbox("Chat Messages", &G::bPrintDamage);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Gameplay");
        ImGui::Separator();
        ImGui::SliderInt("Custom FOV", &G::iFOV, 30, 150);
        //ImGui::Checkbox("Third Person", &G::bThirdPerson);
        ImGui::Checkbox("Auto Join", &G::bAutoJoin);
        ImGui::InputText("IP", G::g_AutoJoinIP, IM_ARRAYSIZE(G::g_AutoJoinIP));

        if (G::bAutoJoin && !strcmp(G::g_AutoJoinIP, ""))
            ImGui::Text("Invalid IP!");
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("MiscCol2", ImVec2(0, 0), true);
    {
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Interface");
        ImGui::Separator();
        ImGui::Checkbox("Crosshair", &G::bCrosshair);
        //ImGui::Checkbox("Spectator List", &G::bSpectatorList);

        /*ImGui::Spacing();
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Configuration");
        ImGui::Separator();*/

        //static char configName[128] = "";
        //ImGui::InputText("Config Name", configName, IM_ARRAYSIZE(configName));

        // Buttons with custom styling
        //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.60f, 0.96f, 0.7f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.68f, 0.99f, 0.9f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.55f, 0.90f, 1.0f));

        //if (ImGui::Button("Save Config", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, 0)))
        //{
        //    // Save configuration logic would go here
        //}

        //ImGui::SameLine();

        //if (ImGui::Button("Load Config", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        //{
        //    // Load configuration logic would go here
        //}

        //ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Other");
        ImGui::Separator();

        const char* listenerText = G::bDisableListeners ? "Listeners are disabled" : "Listeners are active";
        ImGui::Text(listenerText);
        //ImGui::Checkbox("Disable listeners", &G::bDisableListeners);
        ImGui::Checkbox("Hitmarker", &G::bHitmarker);
        ImGui::Checkbox("Hit Sound", &G::bHitSound);
        ImGui::Checkbox("Kill Sound", &G::bKillSound);
    }
    ImGui::EndChild();
    ImGui::EndChild();
}

void RenderDebugTab()
{
#ifdef _DEBUG
    ImGui::Text("Head Position (without bone)");

    ImGui::Text("Current Head Positions:");
    ImGui::Text("X: %f", Debug::vCurrentHeadPos.x);
    ImGui::Text("Y: %f", Debug::vCurrentHeadPos.y);
    ImGui::Text("Z: %f", Debug::vCurrentHeadPos.z);

    ImGui::Separator();

    //ImGui::SliderFloat("X", &Debug::vDebugHeadPos.x, 0.0f, 100.f);
    ImGui::InputFloat("Head X", &Debug::vDebugHeadPos.x);
    ImGui::SameLine();

    Vector3 offset = Debug::vOldHeadPos - Debug::vDebugHeadPos;
    ImGui::Text("Offset: %f", offset.x);

    ImGui::InputFloat("Head Y", &Debug::vDebugHeadPos.y);
    ImGui::SameLine();
    ImGui::Text("Offset: %f", offset.y);

    ImGui::InputFloat("Head Z", &Debug::vDebugHeadPos.z);

    ImGui::SameLine();
    ImGui::Text("Offset: %f", offset.z);

    ImGui::Separator();

    if (ImGui::Button("Reset Positions"))
        Debug::bResetPositions = true;

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("head bone positions");
    ImGui::Separator();
    ImGui::Text("X: %f", Debug::vDebugHeadBone.x);
    ImGui::Text("Y: %f", Debug::vDebugHeadBone.y);
    ImGui::Text("Z: %f", Debug::vDebugHeadBone.z);

    ImGui::Separator();
    ImGui::Text("Enemy Height: %f", G::fEnemyHeight);

    ImGui::SeparatorText("Debug positions");
    ImGui::Spacing();
    ImGui::InputInt("X", &Debug::iX, 50);
    ImGui::InputInt("Y", &Debug::iY, 50);
#endif
    ImGui::SeparatorText("debugcheck");
    ImGui::Text("if nothing is showing, build in debug!");
}

void RenderColorTab()
{
    ImGui::BeginChild("ColorSection", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);
    ImGui::BeginChild("MiscCol1", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, ImGui::GetContentRegionAvail().y), true);
    {
        float childWidth = ImGui::GetContentRegionAvail().x * 0.5f;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::BeginChild("MiscCol1", ImVec2(childWidth, 0), true);
        {
            ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "ESP Colors");
            ImGui::Separator();
            ImGui::ColorEdit3("Enemy", G::fColor, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit3("Target", G::fTargetColor, ImGuiColorEditFlags_NoInputs);

            //static float teamColor[3] = { 0.0f, 1.0f, 0.0f };
            //ImGui::ColorEdit3("Team", teamColor, ImGuiColorEditFlags_NoInputs);
        }
        ImGui::EndChild();

        ImGui::SameLine(0.0f, spacing);

        // Second Column: Crosshair Colors
        ImGui::BeginChild("CrosshairColors", ImVec2(childWidth, 0), true);
        {
            ImGui::TextColored(ImVec4(0.18f, 0.60f, 0.96f, 1.0f), "Crosshair");
            ImGui::Separator();

            ImGui::ColorEdit3("Crosshair", G::fCrosshairColor, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit3("Recoil", G::fRecoilCrosshairColor, ImGuiColorEditFlags_NoInputs);
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
    ImGui::EndChild();
}

void Menu::ShouldRun()
{
	if (GetAsyncKeyState(VK_INSERT) & 1)
		Menu::bShow = !Menu::bShow;

	if (Menu::bShow)
	{
		I::Surface->SetCursorAlwaysVisible(true);
		I::NInputSystem->EnableInput(false);
        RenderNew();
	}
	else
	{
		I::Surface->SetCursorAlwaysVisible(false);
		I::NInputSystem->EnableInput(true);
	}
}

void Menu::Destroy()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
