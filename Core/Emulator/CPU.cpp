#include "CPU.hpp"
#include "Reader.hpp"
#include "Instruction.hpp"
#include "Configuration.hpp"

namespace Core::Emulator
{
    CPU::CPU(const std::string& ROMPath)
    {
        this->LoadROM(ROMPath);
    }

    void CPU::LoadROM(const std::string& ROMPath)
    {
        Reader ROMReader(ROMPath);
        this->Memory.fill(0);
        this->Reset();

        this->Size = ROMReader.Seek(0, std::ios::end);
        ROMReader.Seek(0, std::ios::beg);

        std::vector<std::uint8_t> Bytes = ROMReader.ReadBytes(this->Size);
        for(std::uint32_t i = 0; i < this->Size; i++)
        {
            this->Memory[i + 0x200] = Bytes[i];
        }
        
        std::cout << "Loaded " << this->Size << " Bytes from \"" << ROMPath << "\"\n";
    }

    void CPU::Reset()
    {
        this->Registers.fill(0);
        this->Stack.fill(0);
        this->VideoBuffer.fill({});
        this->ProgramCounter = 0x200;
    }

    void CPU::HandleKeyEvent(sf::Event& Event)
    {
        if(Config::Bindings.find(Event.key.code) != Config::Bindings.end())
        {
            this->Keypad[Config::Bindings[Event.key.code]] = (Event.type == sf::Event::KeyPressed);
        }
    }

    void CPU::CycleTimers()
    {
        if(this->DelayTimer > 0)
        {
            this->DelayTimer--;
        }

        if(this->SoundTimer > 0)
        {
            this->SoundTimer--;
        }
        else
        {
            // Stop Sound;
        }
    }

    void CPU::Cycle()
    {
        this->CurrentOpCode = (this->Memory[this->ProgramCounter] << 8) + this->Memory[this->ProgramCounter + 1];
        Instruction CurrentInstruction(this->CurrentOpCode);

        switch(CurrentInstruction.Type) 
        {
            case 0x0000:
            {
                Instructions::ParseBlock0(this, CurrentInstruction);
                break;
            }
            case 0x1000:
            {
                this->ProgramCounter = CurrentInstruction.nnn;
                break;
            }
            case 0x2000:
            {
                this->Stack[this->StackPointer] = this->ProgramCounter;
                this->StackPointer++;
                this->ProgramCounter = CurrentInstruction.nnn;
                break;
            }
            case 0x3000:
            {
                if(this->Registers[CurrentInstruction.x] == CurrentInstruction.kk)
                {
                    this->ProgramCounter += 2;
                }  
                this->ProgramCounter += 2;
                break;
            }
            case 0x4000:
            {
                if(this->Registers[CurrentInstruction.x] != CurrentInstruction.kk)
                {
                    this->ProgramCounter += 2;
                }  
                this->ProgramCounter += 2;
                break;
            }
            case 0x5000:
            {
                if(this->Registers[CurrentInstruction.x] == this->Registers[CurrentInstruction.y])
                {
                    this->ProgramCounter += 2;
                }  
                this->ProgramCounter += 2;
                break;
            }
            case 0x6000:
            {
                this->Registers[CurrentInstruction.x] = CurrentInstruction.kk;
                this->ProgramCounter += 2;
                break;
            }
            case 0x7000:
            {
                this->Registers[CurrentInstruction.x] += CurrentInstruction.kk;
                this->ProgramCounter += 2;
                break;
            }
            case 0x8000:
            {
                Instructions::ParseBlock8(this, CurrentInstruction);
                break;
            }
            case 0x9000:
            {
                if(this->Registers[CurrentInstruction.x] != this->Registers[CurrentInstruction.y])
                {
                    this->ProgramCounter += 2;
                }
                this->ProgramCounter += 2;
                break;
            }
            case 0xA000:
            {
                this->I = CurrentInstruction.nnn;
                this->ProgramCounter += 2;    
                break;
            }
            case 0xB000:
            {
                this->ProgramCounter = this->Registers[0] + CurrentInstruction.nnn;
                break;
            }
            case 0xC000:
            {
                this->Registers[CurrentInstruction.x] = (rand() % 256) & CurrentInstruction.kk;
                this->ProgramCounter += 2;
                break;
            }
            case 0xD000:
            {
                std::uint16_t X = this->Registers[CurrentInstruction.x];
                std::uint16_t Y = this->Registers[CurrentInstruction.y];
                
                this->Registers[0xF] = 0x0;
                for(std::uint16_t i = 0; i < CurrentInstruction.n; i++)
                {
                    std::uint8_t Byte = this->Memory[this->I + i];
                    for(std::uint8_t j = 0; j < 8; j++)
                    {
                        if(Byte & (0x80 >> j))
                        {
                            if(this->VideoBuffer[X + j][Y + i])
                            {
                                this->Registers[0xF] = 1;
                            }
                            this->VideoBuffer[X + j][Y + i] ^= 1;
                        }
                    }
                }
                
                this->ProgramCounter += 2;
                break;
            }
            case 0xE000:
            {
                Instructions::ParseBlockE(this, CurrentInstruction);
                break;
            }
            case 0xF000:
            {
                Instructions::ParseBlockF(this, CurrentInstruction);
                break;
            }
            default:
            {
                std::cout << std::showbase << std::hex << "Unknown Instruction - " << CurrentInstruction.Type << " | Full OpCode - " << this->CurrentOpCode << "\n";
                break;
            }
        }

        this->TotalTicks++;
    }
}