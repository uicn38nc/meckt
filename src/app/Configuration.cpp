#include "Configuration.hpp"

void Configuration::Initialize() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowResolution = {desktopMode.width, desktopMode.height};

#ifndef DEBUG
    buildVersion = buildVersion + " (debug)";
#endif

    InitializeTextures();
    InitializeFonts();
    InitializeShaders();
}

void Configuration::InitializeTextures() {
    textures.Load(Textures::LOGO, "assets/textures/logo.png");
}

void Configuration::InitializeFonts() {
    fonts.Load(Fonts::FIGTREE, "assets/fonts/Figtree-Medium.ttf");
}

void Configuration::InitializeShaders() {
    shaders.Load(Shaders::PROVINCES, "assets/shaders/provinces.vert", "assets/shaders/provinces.frag");
    // shaders.Load(Shaders::PROVINCES, "assets/shaders/provinces.frag", sf::Shader::Fragment);
}