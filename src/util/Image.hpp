#pragma once

namespace Image {
    sf::Image MapPixels(const sf::Image& originalImage, std::function<void(std::unordered_map<sf::Uint32, sf::Uint32>&)> mapFunc);
}