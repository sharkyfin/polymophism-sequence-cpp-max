#ifndef GUI_VISUALIZER_APP_HPP
#define GUI_VISUALIZER_APP_HPP

#include "imgui.h"

#include "gui/deque_panel.hpp"
#include "gui/matrix_panel.hpp"
#include "gui/ui_helpers.hpp"

inline void DrawVisualizerApp() {
    ImGui::SetNextWindowSize(ImVec2(1120.0f, 760.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Data structure visualizer");

    if (ImGui::BeginTabBar("main-tabs")) {
        if (ImGui::BeginTabItem("Segmented circular deque")) {
            DrawDequeTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Matrices")) {
            DrawMatrixTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Separator();
    DrawStatus();
    ImGui::End();
}

#endif
