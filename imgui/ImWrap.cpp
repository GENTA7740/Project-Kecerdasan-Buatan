#include <ImWrap.h>
#include <vector>
#include <cmath>
ImVec4 ImGui::rbw{ 1.0, 1.0, 1.0, 1.0 };
float ImGui::m_AnimSpeed{ 0.3f };
constexpr float TwoPi = 6.2831853f; // 2π
bool ImGui::m_DrawGradient{ false };
bool ImGui::m_DrawSettingWindow{ false };
int ImGui::m_maxLines = 500;
float ImGui::m_maxDistance = 200.0f;
float maxDistanceSqr = ImGui::m_maxDistance * ImGui::m_maxDistance;
int ImGui::m_dotsCount = 100;
float ImGui::m_circleSize = 3.5f;
float ImGui::m_moveDelay = 0.01f;
float ImGui::m_blinkSpeed = 3.0f;
ImVec4 ImGui::m_ColorAccent { 1.0f, 0.0f, 0.0f, 1.0f  };
ImVec4 ImGui::m_ColorGradient[4] = {
        ImVec4(0.062745f, 0.062745f, 0.250980f, 1.0f), // (16,16,64,255)
        ImVec4(0.188235f, 0.000000f, 0.376471f, 1.0f), // (48,0,96,255)
        ImVec4(0.376471f, 0.000000f, 0.376471f, 1.0f), // (96,0,96,255)
        ImVec4(0.0f, 0.0f, 0.0f, 1.0f)                 // (0,0,0,255)
};
void ImGui::RGBEvent() {
    if (rbw.y < 1 && rbw.z <= 0) {
        rbw.z = 0.0f;
        rbw.y += 0.01f;
    }
    else if (rbw.x > 0 && rbw.y >= 1) {
        rbw.y = 1.0f;
        rbw.x -= 0.01f;
    }
    else if (rbw.z < 1 && rbw.x <= 0) {
        rbw.x = 0.0f;
        rbw.z += 0.01f;
    }
    else if (rbw.y > 0 && rbw.z >= 1) {
        rbw.z = 1.0f;
        rbw.y -= 0.01f;
    }
    else if (rbw.x < 1 && rbw.y <= 0) {
        rbw.y = 0.0f;
        rbw.x += 0.01f;
    }
    else if (rbw.z > 0 && rbw.x >= 1) {
        rbw.x = 1.0f;
        rbw.z -= 0.01f;
    }
}

void ImGui::GentaLegacyColoring(ImGuiStyle* styles) {
    ImVec4* colors = styles->Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.235f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.635f);
    colors[ImGuiCol_Border] = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 0.392f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.0f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.392f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.392f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.392f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 0.392f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.39f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 1.0f, 1.0f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 0.196f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.235f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.235f);
    colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.235f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.235f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.90f, 0.1f, 0.1f, 0.235f);
    colors[ImGuiCol_Separator] = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 0.0f, 0.0f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.196f);
    colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.235f);
    colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.235f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    styles->WindowBorderSize = 0.0f;
    styles->FrameBorderSize = 0.0f;
    styles->TabBorderSize = 1.0f;
    styles->WindowRounding = 10.0f;
    styles->GrabMinSize = 25.0f;
    styles->ScrollbarSize = 20.0f;
    styles->GrabRounding = 0.0f;
    styles->FramePadding = ImVec2(15.f, 10.f);
    styles->WindowTitleAlign = ImVec2(0.5f, 0.5f);
    styles->FrameRounding = 2.0f;
    styles->ColorButtonPosition = ImGuiDir_Left;
}

void ImGui::DynamicVectorGeometryLines() {

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    static std::vector<Particle> particles;
    if (particles.empty() || particles.size() != (size_t)m_dotsCount) {
        particles.clear();
        particles.reserve(m_dotsCount);
        for (int i = 0; i < m_dotsCount; ++i) {
            Particle p;
            p.position = ImVec2(std::rand() % (int)screenSize.x, std::rand() % (int)screenSize.y);
            p.velocity = ImVec2(((std::rand() % 200) - 100) * 0.01f, ((std::rand() % 200) - 100) * 0.01f);
            p.blinkPhase = static_cast<float>(std::rand()) / RAND_MAX * TwoPi;
            particles.push_back(p);
        }
    }

    for (Particle& p : particles) {
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;
        if (p.position.x < 0 || p.position.x > screenSize.x) p.velocity.x *= -1;
        if (p.position.y < 0 || p.position.y > screenSize.y) p.velocity.y *= -1;
        p.blinkPhase += m_blinkSpeed * m_moveDelay;
        if (p.blinkPhase > TwoPi) p.blinkPhase -= TwoPi;
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();


    if (m_DrawGradient) {
        const ImU32 topColor = ImGui::ColorConvertFloat4ToU32(ImGui::m_ColorGradient[0]);
        const ImU32 midColor1 = ImGui::ColorConvertFloat4ToU32(ImGui::m_ColorGradient[1]);
        const ImU32 midColor2 = ImGui::ColorConvertFloat4ToU32(ImGui::m_ColorGradient[2]);
        const ImU32 bottomColor = ImGui::ColorConvertFloat4ToU32(ImGui::m_ColorGradient[3]);
        drawList->AddRectFilledMultiColor(
            ImVec2(0, 0), ImVec2(screenSize.x, screenSize.y / 3),
            topColor, topColor, midColor1, midColor1);

        drawList->AddRectFilledMultiColor(
            ImVec2(0, screenSize.y / 3), ImVec2(screenSize.x, 2 * screenSize.y / 3),
            midColor1, midColor1, midColor2, midColor2);

        drawList->AddRectFilledMultiColor(
            ImVec2(0, 2 * screenSize.y / 3), ImVec2(screenSize.x, screenSize.y),
            midColor2, midColor2, bottomColor, bottomColor);
    }


    int linesDrawn = 0;

    for (size_t i = 0; i < particles.size(); ++i) {
        const Particle& p1 = particles[i];
        float blinkAlpha = (std::sin(p1.blinkPhase) + 1.0f) * 0.5f;
        ImU32 particleColor = IM_COL32(255, 255, 255, static_cast<int>(blinkAlpha * 100.0f));
        drawList->AddCircleFilled(p1.position, m_circleSize, particleColor);

        for (size_t j = i + 1; j < particles.size(); ++j) {
            if (linesDrawn >= m_maxLines) break;

            const Particle& p2 = particles[j];
            float dx = p2.position.x - p1.position.x;
            float dy = p2.position.y - p1.position.y;
            float distSqr = dx * dx + dy * dy;

            if (distSqr < maxDistanceSqr) {
                float dist = std::sqrt(distSqr);
                float alpha = 1.0f - (dist / m_maxDistance);
                ImU32 lineColor = IM_COL32(255, 255, 255, static_cast<int>(alpha * 100.0f));
                drawList->AddLine(p1.position, p2.position, lineColor, 1.0f);
                ++linesDrawn;
            }
        }
    }
}