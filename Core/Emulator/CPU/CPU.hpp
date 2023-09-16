#pragma once
#include <map>
#include <array>
#include <string>
#include <iostream>
#include <cstdint>
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
        std::array<uint8_t, 4096> Memory = {};
        std::array<uint8_t, 16> Registers = {};
        uint16_t I = 0;
        uint8_t DelayTimer = 0;
        uint8_t SoundTimer = 0;
        uint16_t ProgramCounter = 0;
        uint8_t StackPointer = 0;
        std::array<uint16_t, 16> Stack = {};
        std::map<uint8_t, bool> Keypad = {};
        std::array<std::array<uint8_t, 32>, 64> VideoBuffer = {};
        uint64_t Size = 0;
        uint32_t TotalTicks = 0;
        uint16_t CurrentOpCode = 0;
        bool LoadedROM = false;
    };
}