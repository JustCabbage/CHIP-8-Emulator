#pragma once
#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>

namespace Core
{
    struct Config
    {
        std::int32_t CyclesPerFrame = 10;
        std::int32_t RefreshRate = 60;
        std::array<float, 3> Color = {1.f, 1.f, 1.f};
        std::map<sf::Keyboard::Key, std::uint8_t> Bindings =
        {
            {sf::Keyboard::Num1, 0x1},
            {sf::Keyboard::Num2, 0x2},
            {sf::Keyboard::Num3, 0x3},
            {sf::Keyboard::Num4, 0xC},
            {sf::Keyboard::Q, 0x4},
            {sf::Keyboard::W, 0x5},
            {sf::Keyboard::E, 0x6},
            {sf::Keyboard::R, 0xD},
            {sf::Keyboard::A, 0x7},
            {sf::Keyboard::S, 0x8},
            {sf::Keyboard::D, 0x9},
            {sf::Keyboard::F, 0xE},
            {sf::Keyboard::Z, 0xA},
            {sf::Keyboard::X, 0x0},
            {sf::Keyboard::C, 0xB},
            {sf::Keyboard::V, 0xF},
        };
        // Chip-8
        // 1    2    3    C
        // 4    5    6    D
        // 7    8    9    E
        // A    0    B    F
        // Keyboard
        // 1    2    3    4
        // Q    W    E    R
        // A    S    D    F
        // Z    X    C    V
    };
}