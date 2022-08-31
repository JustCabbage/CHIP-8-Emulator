#include "Instruction.hpp"

namespace Core::Emulator
{
    void Instructions::ParseBlock0(CPU* CPU, const Instruction& Instruction)
    {
        switch(Instruction.kk)
        {
            case 0x00E0:
            {
                CPU->VideoBuffer.fill({});
                break;
            }
            case 0x00EE:
            {
                CPU->StackPointer--;
                CPU->ProgramCounter = CPU->Stack[CPU->StackPointer];
                break;
            }
        }
        CPU->ProgramCounter += 2;
    }

    void Instructions::ParseBlock8(CPU* CPU, const Instruction& Instruction)
    {
        switch(Instruction.n)
        {
            case 0x0000:
            {
                CPU->Registers[Instruction.x] = CPU->Registers[Instruction.y];
                break;
            }
            case 0x0001:
            {
                CPU->Registers[Instruction.x] |= CPU->Registers[Instruction.y];
                break;
            }
            case 0x0002:
            {
                CPU->Registers[Instruction.x] &= CPU->Registers[Instruction.y];
                break;
            }
            case 0x0003:
            {
                CPU->Registers[Instruction.x] ^= CPU->Registers[Instruction.y];
                break;
            }
            case 0x0004:
            {
                const std::uint8_t Carry = (CPU->Registers[Instruction.x] + CPU->Registers[Instruction.y]) > 0xFF ? 1 : 0;
                CPU->Registers[Instruction.x] += CPU->Registers[Instruction.y];
                CPU->Registers[0xF] = Carry;
                break;
            }
            case 0x0005:
            {
                const std::uint8_t Borrow = (CPU->Registers[Instruction.x] >= CPU->Registers[Instruction.y]);
                CPU->Registers[Instruction.x] = CPU->Registers[Instruction.x] - CPU->Registers[Instruction.y];
                CPU->Registers[0xF] = Borrow;
                break;
            }
            case 0x0006:
            {
                const std::uint8_t Flag = (CPU->Registers[Instruction.x] & 1) > 0;
                CPU->Registers[Instruction.x] = CPU->Registers[Instruction.y] >> 1;
                CPU->Registers[0xF] = Flag;
                break;
            }
            case 0x0007:
            {
                const std::uint8_t Borrow = CPU->Registers[Instruction.y] > CPU->Registers[Instruction.x] ? 1 : 0;
                CPU->Registers[Instruction.x] = CPU->Registers[Instruction.y] - CPU->Registers[Instruction.x];
                CPU->Registers[0xF] = Borrow;
                break;
            }
            case 0x000E:
            {
                const std::uint8_t Flag = ((CPU->Registers[Instruction.x] & 0x80) > 0);
                CPU->Registers[Instruction.x] = CPU->Registers[Instruction.y] << 1;
                CPU->Registers[0xF] = Flag;
                break;
            }
        }
        CPU->ProgramCounter += 2;
    }

    void Instructions::ParseBlockE(CPU* CPU, const Instruction& Instruction)
    {
        switch(Instruction.kk)
        {
            case 0x009E:
            {
                if(CPU->Keypad[CPU->Registers[Instruction.x]])
                {
                    CPU->ProgramCounter += 2;
                }
                break;
            }
            case 0x00A1:
            {
                if(!CPU->Keypad[CPU->Registers[Instruction.x]])
                {
                    CPU->ProgramCounter += 2;
                }
                break;
            }
        }
        CPU->ProgramCounter += 2;
    }

    void Instructions::ParseBlockF(CPU* CPU, const Instruction& Instruction)
    {
        switch(Instruction.kk)
        {
            case 0x0007:
            {
                CPU->Registers[Instruction.x] = CPU->DelayTimer;
                break;
            }
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
                CPU->ProgramCounter -= 2;
                break;
            }
            case 0x0015:
            {
                CPU->DelayTimer = CPU->Registers[Instruction.x];
                break;
            }
            case 0x0018:
            {
                CPU->SoundTimer = CPU->Registers[Instruction.x];
                if(CPU->SoundTimer > 0)
                {
                    // Play Sound
                }
                break;
            }
            case 0x001E:
            {
                CPU->I += CPU->Registers[Instruction.x];
                break;
            }
            case 0x0029:
            {
                CPU->I = CPU->Registers[Instruction.x] * 0x5;
                break;
            }
            case 0x0033:
            {
                std::uint8_t Value = CPU->Registers[Instruction.x];
                CPU->Memory[CPU->I + 2] = Value % 10;
                Value /= 10;
                CPU->Memory[CPU->I + 1] = Value % 10;
                CPU->Memory[CPU->I] = Value / 10;
                break;
            }
            case 0x0055:
            {
                for(std::uint8_t i = 0; i <= Instruction.x; i++)
                {
                    CPU->Memory[CPU->I + i] = CPU->Registers[i];
                }
                CPU->I += Instruction.x;
                break;
            }
            case 0x0065:
            {
                for(std::uint8_t i = 0; i <= Instruction.x; i++)
                {
                    CPU->Registers[i] = CPU->Memory[CPU->I + i];
                }
                CPU->I += Instruction.x;
                break;
            }
        }
        CPU->ProgramCounter += 2;
    }
}