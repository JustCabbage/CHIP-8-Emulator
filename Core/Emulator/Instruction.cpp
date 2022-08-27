#include "Instruction.hpp"

namespace Core::Emulator
{
    void Instructions::ParseBlock0(CPU* CPU, Instruction& Instruction)
    {
        switch(Instruction.kk)
        {
            case 0x00E0:
            {
                std::array<std::uint8_t, 32> EmptyRow;
                EmptyRow.fill(0);
                CPU->VideoBuffer.fill(EmptyRow);
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x00EE:
            {
                CPU->StackPointer--;
                CPU->ProgramCounter = CPU->Stack[CPU->StackPointer];
                CPU->ProgramCounter += 2;
                break;
            }
        }
    }

    void Instructions::ParseBlock8(CPU* CPU, Instruction& Instruction)
    {
        switch(Instruction.n)
        {
            case 0x0000:
            {
                CPU->Registers[Instruction.x] = CPU->Registers[Instruction.y];
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x0001:
            {
                CPU->Registers[Instruction.x] |= CPU->Registers[Instruction.y];
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x0003:
            {
                CPU->Registers[Instruction.x] ^= CPU->Registers[Instruction.y];
                CPU->ProgramCounter += 2;
                break;
            }
        }
    }

    void Instructions::ParseBlockE(CPU* CPU, Instruction& Instruction)
    {
        switch(Instruction.kk)
        {
            case 0x00A1:
            {
                if(!CPU->Keypad[Instruction.x])
                {
                    CPU->ProgramCounter += 2;
                }
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x009E:
            {
                if(CPU->Keypad[Instruction.x])
                {
                    CPU->ProgramCounter += 2;
                }
                CPU->ProgramCounter += 2;
                break;
            }
        }
    }

    void Instructions::ParseBlockF(CPU* CPU, Instruction& Instruction)
    {
        switch(Instruction.kk)
        {
            case 0x000A:
            {
                for(std::uint8_t i = 0; i < CPU->Keypad.size(); i++)
                {
                    if(CPU->Keypad[i])
                    {
                        CPU->Registers[Instruction.x] = i;
                        CPU->ProgramCounter += 2;
                        break;
                    }
                }
                break;
            }
            case 0x0007:
            {
                CPU->Registers[Instruction.x] = CPU->DelayTimer;
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x0015:
            {
                CPU->DelayTimer = CPU->Registers[Instruction.x];
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x0055:
            {
                for(std::uint8_t i = 0; i <= Instruction.x; i++)
                {
                    CPU->Memory[CPU->I + i] = CPU->Registers[i];
                }
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x0065:
            {
                for(std::uint8_t i = 0; i <= Instruction.x; i++)
                {
                    CPU->Registers[i] = CPU->Memory[CPU->I + i];
                }
                CPU->ProgramCounter += 2;
                break;
            }
            case 0x001E:
            {
                CPU->I += CPU->Registers[Instruction.x];
                CPU->Registers[0xF] = CPU->I > 0x0FFF ? 1 : 0; 
                CPU->ProgramCounter += 2;
                break;
            }
        }
    }
}