#pragma once
#include <map>
#include <array>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

namespace Core::Emulator
{
    class CPU
    {
    public:
        CPU(const std::string& ROMPath);
        void Reset();
        void HandleKeyEvent(sf::Event& Event);
        void CycleTimers();
        void Cycle();
    public:
        std::uint16_t I = 0;
        std::uint32_t Size = 0;
        std::uint8_t DelayTimer = 0;
        std::uint64_t TotalTicks = 0;
        std::uint8_t StackPointer = 0;
        std::uint16_t CurrentOpCode = 0;
        std::uint16_t m_ProgramCounter = 0;
        std::array<std::uint16_t, 16> Stack = {};
        std::map<std::uint16_t, bool> Keypad = {};
        std::array<std::uint8_t, 4096> Memory = {};
        std::array<std::uint8_t, 16> Registers = {};
        std::array<std::array<std::uint8_t, 32>, 64> VideoBuffer = {};
    };
}