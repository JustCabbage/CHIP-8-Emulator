#include "CPU.hpp"
#include "Reader.hpp"

namespace Core::Emulator
{
    CPU::CPU(const std::string& ROMPath)
    {
        Reader ROMReader(ROMPath);

        this->m_Memory.fill(0);
        this->m_Registers.fill(0);
        std::array<std::uint8_t, 32> EmptyRow;
        EmptyRow.fill(0);
        this->VideoBuffer.fill(EmptyRow);
        this->m_ProgramCounter = 0x200;

        this->m_Size = ROMReader.Seek(0, std::ios::end);
        ROMReader.Seek(0, std::ios::beg);

        std::vector<std::uint8_t> Bytes = ROMReader.ReadBytes(this->m_Size);

        for(std::uint32_t i = 0; i < this->m_Size; i++)
        {
            this->m_Memory[i + 0x200] = Bytes[i];
        }
        
        std::cout << "Loaded " << this->m_Size << " Bytes from \"" << ROMPath << "\"\n"; 
    }

    std::uint16_t CPU::GetCurrentOpCode()
    {
        return this->m_CurrentOpCode;
    }
    
    std::array<std::uint8_t, 16> CPU::GetRegisters()
    {
        return this->m_Registers;
    }

    void CPU::Cycle()
    {
        this->m_CurrentOpCode = (this->m_Memory[this->m_ProgramCounter] << 8) + this->m_Memory[this->m_ProgramCounter + 1];
        std::uint16_t Instruction = this->m_CurrentOpCode & 0xF000;

        switch(Instruction) 
        {
            case 0x0000:
            {
                std::uint16_t Postfix = this->m_CurrentOpCode & 0x00FF;
                switch(Postfix)
                {
                    case 0x00E0:
                    {
                        std::array<std::uint8_t, 32> EmptyRow;
                        EmptyRow.fill(0);
                        this->VideoBuffer.fill(EmptyRow);
                        this->m_ProgramCounter += 2;
                        break;
                    }
                }
                break;
            }
            case 0x1000:
            {
                std::uint16_t JumpLocation = this->m_CurrentOpCode & 0x0FFF;
                this->m_ProgramCounter = JumpLocation;
                break;
            }
            case 0x6000:
            {
                std::uint16_t RegisterIndex = (this->m_CurrentOpCode & 0x0F00) >> 8;
                std::uint8_t Value = this->m_CurrentOpCode & 0x00FF;
                this->m_Registers[RegisterIndex] = Value;
                this->m_ProgramCounter += 2;
                break;
            }
            case 0x7000:
            {
                std::uint16_t RegisterIndex = (this->m_CurrentOpCode & 0x0F00) >> 8;
                std::uint8_t Value = this->m_CurrentOpCode & 0x00FF;
                this->m_Registers[RegisterIndex] += Value;
                this->m_ProgramCounter += 2;
                break;
            }
            case 0xA000:
            {
                std::uint16_t Value = this->m_CurrentOpCode & 0x0FFF;
                this->m_I = Value;
                this->m_ProgramCounter += 2;    
                break;
            }
            case 0xD000:
            {
                std::uint16_t X = this->m_Registers[(this->m_CurrentOpCode & 0x0F00) >> 8];
                std::uint16_t Y = this->m_Registers[(this->m_CurrentOpCode & 0x00F0) >> 4];
                std::uint16_t PixelHeight = this->m_CurrentOpCode & 0x000F;
                this->m_Registers[0xF] = 0x0;
                for(int i = 0; i < PixelHeight; i++)
                {
                    std::uint8_t Byte = this->m_Memory[this->m_I + i];
                    for(int j = 0; j < 8; j++)
                    {
                        if(Byte & (0x80 >> j))
                        {
                            this->VideoBuffer[X + j][Y + i] = 1;
                        }
                    }
                }
                this->m_ProgramCounter += 2;
                break;
            }
            default:
            {
                std::cout << std::showbase << std::hex << "Unknown Instruction - " << Instruction << " | Full OpCode - " << this->m_CurrentOpCode << "\n";
                break;
            }
        }
        
    }
}