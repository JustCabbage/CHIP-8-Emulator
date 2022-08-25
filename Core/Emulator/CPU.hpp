#pragma once
#include <array>
#include <string>
#include <iostream>

namespace Core::Emulator
{
    class CPU
    {
    public:
        CPU(const std::string& ROMPath);
        std::uint16_t GetCurrentOpCode();
        std::array<std::uint8_t, 16> GetRegisters();
        void Cycle();
    public:
        std::array<std::array<std::uint8_t, 32>, 64> VideoBuffer = {};
    private:
        std::uint16_t m_I = 0;
        std::uint64_t m_Size = 0;
        std::uint16_t m_CurrentOpCode = 0;
        std::uint16_t m_ProgramCounter = 0;
        std::array<std::uint8_t, 4096> m_Memory = {};
        std::array<std::uint8_t, 16> m_Registers = {};
    };
}