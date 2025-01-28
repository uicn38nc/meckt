#include "HomeMenu.hpp"
#include "EditorMenu.hpp"
#include "app/App.hpp"
#include "imgui/imgui.hpp"
#include "nfd/nfd.h"

HomeMenu::HomeMenu(App* app)
: Menu(app, "Home") {}

void HomeMenu::Update(sf::Time delta) {

}

void HomeMenu::Event(const sf::Event& event) {

}

void HomeMenu::Render() {
    static std::string error = "";

    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(m_App->GetWindow().getSize().x - 20, m_App->GetWindow().getSize().y - 20), ImGuiCond_Always);
    ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImGui::GetWindowPos();

    float spacing = 20.0f;
    ImVec2 buttonSize = ImVec2(200.0f, 50.0f);

    const sf::Texture& logoTexture = Configuration::textures.Get(Textures::LOGO);

    float totalHeight = logoTexture.getSize().y + buttonSize.y * 2 + spacing * 2;
    float startY = (windowSize.y - totalHeight) * 0.4f + windowPos.y;
    float centerX = windowSize.x * 0.5f + windowPos.x;

    ImGui::SetCursorPos(ImVec2(centerX - logoTexture.getSize().x*0.5f, startY));
    ImGui::Image(logoTexture);

    ImVec2 textSize = ImGui::CalcTextSize(error.c_str());
    ImGui::SetCursorPos(ImVec2(centerX - textSize.x*0.5f, startY + logoTexture.getSize().y - textSize.y*0.5f));
    if(!error.empty()) {
        ImGui::TextColored(ImVec4(1.0, 0, 0, 1.0), error.c_str());
    }

    ImGui::SetCursorPos(ImVec2(centerX - buttonSize.x*0.5f, startY + logoTexture.getSize().y + spacing));
    if(ImGui::Button("Open mod", buttonSize)) {
        nfdchar_t *dirPath = NULL;
        nfdresult_t result = NFD_PickFolder(NULL, &dirPath);
            
        if(result == NFD_OKAY) {
            SharedPtr<Mod> mod = MakeShared<Mod>(std::string(dirPath));
            if(mod->HasMap()) {
                m_App->OpenMod(mod);
                LOG_INFO("Opened mod at {}", dirPath);
            }
            else {
                LOG_INFO("Opened mod without custom map at {}", dirPath);
                error = fmt::format("This mod does not have a custom map.");
            }
            free(dirPath);
        }
        else if(result != NFD_CANCEL) {
            LOG_ERROR("Failed to open mod at {}", NFD_GetError());
            error = fmt::format("Failed to open mod ({})", NFD_GetError());
        }
    }
    if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Select a mod directory (with descriptor file)");
    }

    ImGui::SetCursorPos(ImVec2(centerX - buttonSize.x*0.5f, startY + logoTexture.getSize().y + spacing * 2 + buttonSize.y));
    if(ImGui::Button("Exit", buttonSize)) {
        m_App->GetWindow().close();
    }

    textSize = ImGui::CalcTextSize(Configuration::buildVersion.c_str());
    ImGui::SetCursorPos(ImVec2(5.0f, windowSize.y - textSize.y - 5.0f));
    ImGui::Text(Configuration::buildVersion.c_str());

    textSize = ImGui::CalcTextSize(Configuration::buildCredits.c_str());
    ImVec2 creditsPos = ImVec2(windowSize.x - textSize.x - 5.0f, windowSize.y - textSize.y - 5.0f);
    ImGui::SetCursorPos(creditsPos);
    creditsPos = ImGui::GetCursorScreenPos();
    ImGui::Text(Configuration::buildCredits.c_str());
    if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        std::string command;
#ifdef _WIN32
        command = "start " + Configuration::githubURL;
#else
        command = "xdg-open " + Configuration::githubURL + "&>/dev/null";
#endif
        if(std::system(command.c_str())) {}
    }
    if(ImGui::IsItemHovered()) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddLine(
            ImVec2(creditsPos.x - 2.0f, creditsPos.y + textSize.y + 0.5f),
            ImVec2(creditsPos.x + textSize.x + 2.0f, creditsPos.y + textSize.y + 0.5f),
            ImGui::GetColorU32(ImGuiCol_Text),
            1.0f
        );
    }

    ImGui::End();

}