#pragma once
#include <map>
#include <array>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../Configuration.hpp"


namespace Core::Emulator
{
    class CPU
    {
    public:
        CPU() {}
        void LoadROM(const std::string& ROMPath);
        void Reset();
        void HandleKeyEvent(sf::Event& Event, Config& Settings);
        void CycleTimers();
        void Cycle();
    public:
        std::array<std::uint8_t, 4096> Memory = {};
        std::array<std::uint8_t, 16> Registers = {};
        std::uint16_t I = 0;
        std::uint8_t DelayTimer = 0;
        std::uint8_t SoundTimer = 0;
        std::uint16_t ProgramCounter = 0;
        std::uint8_t StackPointer = 0;
        std::array<std::uint16_t, 16> Stack = {};
        std::map<std::uint8_t, bool> Keypad = {};
        std::array<std::array<std::uint8_t, 32>, 64> VideoBuffer = {};
        std::uint64_t Size = 0;
        std::uint64_t TotalTicks = 0;
        std::uint16_t CurrentOpCode = 0;
        bool LoadedROM = false;
    };
}