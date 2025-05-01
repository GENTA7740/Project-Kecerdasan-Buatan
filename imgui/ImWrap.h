#pragma once
#include "imgui_internal.h"
struct Particle {
    ImVec2 position;
    ImVec2 velocity;
    float blinkPhase{};
};

namespace ImGui {
    extern ImVec4 rbw;
    extern float m_AnimSpeed;
    extern bool m_DrawGradient;
    extern bool m_DrawSettingWindow;
    extern int m_maxLines;
    extern float m_maxDistance;
    extern int m_dotsCount;
    extern float m_circleSize;
    extern float m_moveDelay;
    extern float m_blinkSpeed;
    extern ImVec4 m_ColorAccent;
    extern ImVec4 m_ColorGradient[4];
    void DynamicVectorGeometryLines();
    void RGBEvent();
    void GentaLegacyColoring(ImGuiStyle* styles);
}