#ifndef GUI_MATRIX_PANEL_HPP
#define GUI_MATRIX_PANEL_HPP

#include <cstdio>

#include "imgui.h"

#include "gui/ui_helpers.hpp"

inline void DrawMatrixIntInput(const char* label, int* value) {
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputInt(label, value);
}

inline void DrawMatrixDoubleInput(const char* label, double* value) {
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputDouble(label, value);
}

inline int MatrixRows(const GuiState& state) {
    switch (state.matrixMode) {
        case MatrixViewRectangular:
            return state.rectangular.GetRows();
        case MatrixViewSquare:
            return state.square.GetSize();
        case MatrixViewLowerTriangular:
            return state.lowerTriangular.GetSize();
        case MatrixViewUpperTriangular:
            return state.upperTriangular.GetSize();
        case MatrixViewDiagonal:
            return state.diagonal.GetSize();
    }
    return 0;
}

inline int MatrixCols(const GuiState& state) {
    switch (state.matrixMode) {
        case MatrixViewRectangular:
            return state.rectangular.GetCols();
        case MatrixViewSquare:
            return state.square.GetSize();
        case MatrixViewLowerTriangular:
            return state.lowerTriangular.GetSize();
        case MatrixViewUpperTriangular:
            return state.upperTriangular.GetSize();
        case MatrixViewDiagonal:
            return state.diagonal.GetSize();
    }
    return 0;
}

inline double MatrixValue(const GuiState& state, int row, int col) {
    switch (state.matrixMode) {
        case MatrixViewRectangular:
            return state.rectangular.Get(row, col);
        case MatrixViewSquare:
            return state.square.Get(row, col);
        case MatrixViewLowerTriangular:
            return state.lowerTriangular.Get(row, col);
        case MatrixViewUpperTriangular:
            return state.upperTriangular.Get(row, col);
        case MatrixViewDiagonal:
            return state.diagonal.Get(row, col);
    }
    return 0.0;
}

inline bool MatrixCellEditable(const GuiState& state, int row, int col) {
    switch (state.matrixMode) {
        case MatrixViewLowerTriangular:
            return row >= col;
        case MatrixViewUpperTriangular:
            return row <= col;
        case MatrixViewDiagonal:
            return row == col;
        default:
            return true;
    }
}

inline ImVec4 MatrixEditableColor() {
    return ImVec4(0.15f, 0.34f, 0.28f, 1.0f);
}

inline ImVec4 MatrixDiagonalColor() {
    return ImVec4(0.28f, 0.42f, 0.22f, 1.0f);
}

inline ImVec4 MatrixNonEditableColor() {
    return ImVec4(0.13f, 0.14f, 0.16f, 1.0f);
}

inline ImVec4 MatrixNonEditableDiagonalColor() {
    return ImVec4(0.18f, 0.18f, 0.16f, 1.0f);
}

inline void RebuildMatrices() {
    GuiState& state = State();
    if (state.rectRows < 1) {
        state.rectRows = 1;
    }
    if (state.rectCols < 1) {
        state.rectCols = 1;
    }
    if (state.matrixSize < 1) {
        state.matrixSize = 1;
    }

    state.rectangular = CreateRectangularSample(state.rectRows, state.rectCols);
    state.square = CreateSquareSample(state.matrixSize);
    state.lowerTriangular = CreateTriangularSample(state.matrixSize, TriangleKind::Lower);
    state.upperTriangular = CreateTriangularSample(state.matrixSize, TriangleKind::Upper);
    state.diagonal = CreateDiagonalSample(state.matrixSize);
    SetStatus("Matrices rebuilt");
}

inline void SetMatrixCell() {
    GuiState& state = State();
    switch (state.matrixMode) {
        case MatrixViewRectangular:
            state.rectangular.Set(state.editRow, state.editCol, state.editValue);
            break;
        case MatrixViewSquare:
            state.square.Set(state.editRow, state.editCol, state.editValue);
            break;
        case MatrixViewLowerTriangular:
            state.lowerTriangular.Set(state.editRow, state.editCol, state.editValue);
            break;
        case MatrixViewUpperTriangular:
            state.upperTriangular.Set(state.editRow, state.editCol, state.editValue);
            break;
        case MatrixViewDiagonal:
            if (state.editRow != state.editCol) {
                throw InvalidArgumentException("DiagonalMatrix: edit row must equal column");
            }
            state.diagonal.SetDiagonal(state.editRow, state.editValue);
            break;
    }
}

inline void DrawMatrixCell(const GuiState& state, int row, int col) {
    bool editable = MatrixCellEditable(state, row, col);
    double value = MatrixValue(state, row, col);

    ImVec4 color = editable ? MatrixEditableColor() : MatrixNonEditableColor();
    if (row == col) {
        color = editable ? MatrixDiagonalColor() : MatrixNonEditableDiagonalColor();
    }

    char label[48];
    std::snprintf(label, sizeof(label), "%.2f", value);

    ImGui::PushID(row * 1000 + col);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Hovered(color));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, Hovered(color));
    if (ImGui::Button(label, ImVec2(74.0f, 30.0f))) {
        GuiState& mutableState = State();
        mutableState.editRow = row;
        mutableState.editCol = col;
        mutableState.editValue = value;
    }
    ImGui::PopStyleColor(3);

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("row: %d", row);
        ImGui::Text("col: %d", col);
        ImGui::Text("value: %.4f", value);
        ImGui::TextUnformatted(editable ? "editable" : "non editable");
        ImGui::EndTooltip();
    }
    ImGui::PopID();
}

inline void DrawMatrixLegend() {
    ImGui::TextColored(MatrixDiagonalColor(), "diagonal");
    ImGui::SameLine();
    ImGui::TextColored(MatrixEditableColor(), "editable");
    ImGui::SameLine();
    ImGui::TextColored(MatrixNonEditableColor(), "non editable");
}

inline void DrawMatrixTable(const GuiState& state) {
    int rows = MatrixRows(state);
    int cols = MatrixCols(state);

    ImGuiTableFlags flags = ImGuiTableFlags_Borders |
                            ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable("matrix-table", cols + 1, flags, ImVec2(0, 360))) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted("row");
        for (int col = 0; col < cols; ++col) {
            ImGui::TableSetColumnIndex(col + 1);
            ImGui::Text("%d", col);
        }

        for (int row = 0; row < rows; ++row) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", row);
            for (int col = 0; col < cols; ++col) {
                ImGui::TableSetColumnIndex(col + 1);
                DrawMatrixCell(state, row, col);
            }
        }

        ImGui::EndTable();
    }
}

inline void DrawMatrixTab() {
    GuiState& state = State();

    const char* modes[] = {
        "Rectangular",
        "Square",
        "Lower triangular",
        "Upper triangular",
        "Diagonal"
    };

    ImGui::SetNextItemWidth(180.0f);
    ImGui::Combo("type", &state.matrixMode, modes, 5);

    if (state.matrixMode == MatrixViewRectangular) {
        DrawMatrixIntInput("rows", &state.rectRows);
        ImGui::SameLine();
        DrawMatrixIntInput("cols", &state.rectCols);
    } else {
        DrawMatrixIntInput("size", &state.matrixSize);
    }

    if (ImGui::Button("Rebuild samples")) {
        RunAction([]() { RebuildMatrices(); }, "Matrices rebuilt");
    }

    ImGui::Separator();
    DrawMatrixIntInput("row", &state.editRow);
    ImGui::SameLine();
    DrawMatrixIntInput("col", &state.editCol);
    DrawMatrixDoubleInput("value", &state.editValue);
    if (ImGui::Button("Set cell")) {
        RunAction([]() { SetMatrixCell(); }, "Cell updated");
    }

    ImGui::Spacing();
    ImGui::Text("rows: %d", MatrixRows(state));
    ImGui::SameLine();
    ImGui::Text("cols: %d", MatrixCols(state));

    DrawMatrixLegend();

    DrawMatrixTable(state);
}

#endif
