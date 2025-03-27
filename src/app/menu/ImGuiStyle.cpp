#include "ImGuiStyle.hpp"

void ImGui::SetupSettings() {
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Disable saving windows settings
    // io.IniFilename = NULL;
    // io.LogFilename = NULL;
}

void ImGui::SetupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}

bool ImGui::ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags) {
    float col4[4] = { color->r/255.f, color->g/255.f, color->b/255.f, 1.0f };
    if (!ColorEdit4(label, col4, flags | ImGuiColorEditFlags_NoAlpha))
        return false;
    color->r = col4[0]*255.f; color->g = col4[1]*255.f; color->b = col4[2]*255.f;
    return true;
}

// https://github.com/ocornut/imgui/issues/2644
bool ImGui::CheckBoxTristate(const char* label, int* v_tristate) {
    bool ret;
    if (*v_tristate == -1) {
        ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
        bool b = false;
        ret = ImGui::Checkbox(label, &b);
        if (ret) *v_tristate = 1;
        ImGui::PopItemFlag();
    }
    else {
        bool b = (*v_tristate != 0);
        ret = ImGui::Checkbox(label, &b);
        if (ret) *v_tristate = (int)b;
    }
    return ret;
}