#ifndef GUI_DEQUE_PANEL_HPP
#define GUI_DEQUE_PANEL_HPP

#include <cstdio>
#include <string>

#include "imgui.h"

#include "gui/ui_helpers.hpp"

inline bool DequeSegmentHasActiveCells(const Deque<int>& deque, int segment) {
    for (int offset = 0; offset < deque.DebugGetSegmentSize(); ++offset) {
        if (deque.DebugIsActiveCell(segment, offset)) {
            return true;
        }
    }
    return false;
}

inline int DequeSegmentLogicalOrder(const Deque<int>& deque, int segment) {
    if (!DequeSegmentHasActiveCells(deque, segment)) {
        return -1;
    }

    int mapSize = deque.DebugGetMapSize();
    int order = segment - deque.DebugGetFirstSegment();
    if (order < 0) {
        order += mapSize;
    }
    return order;
}

inline void DrawDequeCell(const Deque<int>& deque, int segment, int offset) {
    int logicalIndex = deque.DebugGetLogicalIndex(segment, offset);
    bool active = logicalIndex >= 0;
    bool hasSegment = deque.DebugHasSegment(segment);

    ImVec4 color = hasSegment ? ImVec4(0.16f, 0.18f, 0.22f, 1.0f)
                              : ImVec4(0.08f, 0.09f, 0.11f, 1.0f);
    if (active) {
        color = ImVec4(0.12f, 0.38f, 0.62f, 1.0f);
    }
    if (logicalIndex == 0) {
        color = ImVec4(0.76f, 0.54f, 0.13f, 1.0f);
    } else if (logicalIndex == deque.GetLength() - 1) {
        color = ImVec4(0.62f, 0.28f, 0.52f, 1.0f);
    }

    char label[32];
    if (active) {
        std::snprintf(label, sizeof(label), "%d", deque.DebugGetRawCell(segment, offset));
    } else if (hasSegment) {
        std::snprintf(label, sizeof(label), ".");
    } else {
        std::snprintf(label, sizeof(label), " ");
    }

    ImGui::PushID(segment * 1000 + offset);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Hovered(color));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Hovered(color));
    ImGui::Button(label, ImVec2(46.0f, 28.0f));
    ImGui::PopStyleColor(3);

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("segment: %d", segment);
        ImGui::Text("offset: %d", offset);
        if (active) {
            ImGui::Text("logical index: %d", logicalIndex);
            ImGui::Text("value: %d", deque.DebugGetRawCell(segment, offset));
        } else {
            ImGui::TextUnformatted(hasSegment ? "allocated" : "not allocated");
        }
        ImGui::EndTooltip();
    }
    ImGui::PopID();
}

inline void DrawDequeMapStrip(const Deque<int>& deque) {
    int mapSize = deque.DebugGetMapSize();
    if (mapSize <= 0) {
        return;
    }

    for (int segment = 0; segment < mapSize; ++segment) {
        bool active = DequeSegmentHasActiveCells(deque, segment);
        bool first = segment == deque.DebugGetFirstSegment();

        ImVec4 color = active ? ImVec4(0.12f, 0.38f, 0.62f, 1.0f)
                              : ImVec4(0.13f, 0.14f, 0.16f, 1.0f);
        if (first) {
            color = ImVec4(0.76f, 0.54f, 0.13f, 1.0f);
        }

        char label[24];
        int order = DequeSegmentLogicalOrder(deque, segment);
        if (order >= 0) {
            std::snprintf(label, sizeof(label), "%d:%d", segment, order);
        } else {
            std::snprintf(label, sizeof(label), "%d", segment);
        }

        ImGui::PushID(20000 + segment);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Hovered(color));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Hovered(color));
        ImGui::Button(label, ImVec2(48.0f, 28.0f));
        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("physical segment: %d", segment);
            if (order >= 0) {
                ImGui::Text("logical segment order: %d", order);
            } else {
                ImGui::TextUnformatted("inactive segment");
            }
            ImGui::EndTooltip();
        }
        ImGui::PopID();

        if (segment + 1 < mapSize) {
            ImGui::SameLine();
        }
    }
}

inline void DrawDequeSegments(const Deque<int>& deque) {
    int mapSize = deque.DebugGetMapSize();
    int segmentSize = deque.DebugGetSegmentSize();
    if (mapSize <= 0 || segmentSize <= 0) {
        ImGui::TextUnformatted("Deque storage is empty.");
        return;
    }

    ImGuiTableFlags flags = ImGuiTableFlags_Borders |
                            ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("deque-segments", segmentSize + 2, flags, ImVec2(0, 360))) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("segment");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted("order");
        for (int offset = 0; offset < segmentSize; ++offset) {
            ImGui::TableSetColumnIndex(offset + 2);
            ImGui::Text("%d", offset);
        }

        for (int segment = 0; segment < mapSize; ++segment) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            bool firstSegment = segment == deque.DebugGetFirstSegment();
            ImGui::Text("%s%d", firstSegment ? ">" : " ", segment);
            ImGui::TableSetColumnIndex(1);
            int order = DequeSegmentLogicalOrder(deque, segment);
            if (order >= 0) {
                ImGui::Text("%d", order);
            } else {
                ImGui::TextUnformatted("-");
            }

            for (int offset = 0; offset < segmentSize; ++offset) {
                ImGui::TableSetColumnIndex(offset + 2);
                DrawDequeCell(deque, segment, offset);
            }
        }

        ImGui::EndTable();
    }
}

inline void DrawDequeTab() {
    GuiState& state = State();

    ImGui::TextUnformatted("Deque<int>");
    ImGui::Separator();

    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputInt("value", &state.dequeValue);
    if (ImGui::Button("Append")) {
        RunAction([&]() { state.deque.Append(state.dequeValue); }, "Append completed");
    }
    ImGui::SameLine();
    if (ImGui::Button("Prepend")) {
        RunAction([&]() { state.deque.Prepend(state.dequeValue); }, "Prepend completed");
    }
    ImGui::SameLine();
    if (ImGui::Button("PopFront")) {
        RunAction([&]() { state.deque.PopFront(); }, "PopFront completed");
    }
    ImGui::SameLine();
    if (ImGui::Button("PopBack")) {
        RunAction([&]() { state.deque.PopBack(); }, "PopBack completed");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset sample")) {
        state.deque = CreateSampleDeque();
        SetStatus("Deque sample reset");
    }

    ImGui::Spacing();
    ImGui::Text("length: %d", state.deque.GetLength());
    ImGui::SameLine();
    ImGui::Text("map size: %d", state.deque.DebugGetMapSize());
    ImGui::SameLine();
    ImGui::Text("segment size: %d", state.deque.DebugGetSegmentSize());
    ImGui::SameLine();
    ImGui::Text("first: segment %d, offset %d",
                state.deque.DebugGetFirstSegment(),
                state.deque.DebugGetFirstIndex());

    ImGui::TextColored(ImVec4(0.76f, 0.54f, 0.13f, 1.0f), "first");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.62f, 0.28f, 0.52f, 1.0f), "last");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.12f, 0.38f, 0.62f, 1.0f), "active");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.36f, 0.42f, 0.50f, 1.0f), "allocated");

    DrawDequeMapStrip(state.deque);
    ImGui::Spacing();
    DrawDequeSegments(state.deque);
}

#endif
