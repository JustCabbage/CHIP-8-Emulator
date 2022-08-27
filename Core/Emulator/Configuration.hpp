#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

namespace Core::Config
{
    inline std::int32_t CyclesPerFrame = 1;
    inline std::map<sf::Keyboard::Key, std::uint8_t> Bindings =
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
}