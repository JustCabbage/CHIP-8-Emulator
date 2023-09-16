#pragma once
#include <iostream>
#include "../CPU.hpp"

namespace Core::Emulator
{
    struct Instruction
    {
        // nnn - A 12-bit value, the lowest 12 bits of the instruction
        // n - A 4-bit value, the lowest 4 bits of the instruction
        // x - A 4-bit value, the lower 4 bits of the high byte of the instruction
        // y - A 4-bit value, the upper 4 bits of the low byte of the instruction
        // kk - An 8-bit value, the lowest 8 bits of the instruct

        explicit Instruction(uint16_t OpCode)
        {
            this->Type = OpCode & 0xF000;
            this->nnn = OpCode & 0x0FFF;
            this->n = OpCode & 0x000F;
            this->x = (OpCode & 0x0F00) >> 8;
            this->y = (OpCode & 0x00F0) >> 4;
            this->kk = OpCode & 0x00FF;
        }

        uint16_t Type;
        uint8_t n;
        uint8_t x, y;
        uint8_t kk;
        uint16_t nnn;
    };

    namespace Instructions
    {
        void ParseBlock0(CPU* CPU, const Instruction& Instruction);
        void ParseBlock8(CPU* CPU, const Instruction& Instruction);
        void ParseBlockE(CPU* CPU, const Instruction& Instruction);
        void ParseBlockF(CPU* CPU, const Instruction& Instruction);
    }
}
