#ifndef GUI_UI_HELPERS_HPP
#define GUI_UI_HELPERS_HPP

#include "imgui.h"

#include "core/exceptions.hpp"
#include "gui/gui_state.hpp"

inline ImVec4 Hovered(ImVec4 color) {
    color.x += (1.0f - color.x) * 0.14f;
    color.y += (1.0f - color.y) * 0.14f;
    color.z += (1.0f - color.z) * 0.14f;
    return color;
}

inline void DrawStatus() {
    GuiState& state = State();
    ImVec4 color = state.statusIsError ? ImVec4(1.0f, 0.35f, 0.25f, 1.0f)
                                       : ImVec4(0.35f, 0.9f, 0.65f, 1.0f);
    ImGui::TextColored(color, "%s", state.status.c_str());
}

template <class Action>
void RunAction(Action action, const char* okMessage) {
    try {
        action();
        SetStatus(okMessage);
    } catch (const LabException& error) {
        SetStatus(error.GetMessage(), true);
    } catch (...) {
        SetStatus("Unknown error", true);
    }
}

#endif
