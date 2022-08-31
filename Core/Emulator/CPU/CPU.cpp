#include "Instruction/Instruction.hpp"
#include "../Configuration.hpp"
#include "../Reader.hpp"
#include "CPU.hpp"

namespace Core::Emulator
{
    void CPU::LoadROM(const std::string& ROMPath)
    {
        Reader ROMReader;
        if (!ROMReader.SetStream(ROMPath))
        {
            return;
        }
        
        this->Reset();

        this->Size = ROMReader.Seek(0, std::ios::end);
        ROMReader.Seek(0, std::ios::beg);

        const std::vector<std::uint8_t> Bytes = ROMReader.ReadBytes(this->Size);
        for (std::uint32_t i = 0; i < this->Size; i++)
        {
            this->Memory[i + 0x200] = Bytes[i];
        }
        
        std::cout << "Loaded " << this->Size << " Bytes from \"" << ROMPath << "\"\n";
        this->LoadedROM = true;
    }

    void CPU::Reset()
    {
        this->Memory.fill(0);
        this->Registers.fill(0);
        this->I = 0;
        this->DelayTimer = 0;
        this->SoundTimer = 0;
        this->ProgramCounter = 0x200;
        this->StackPointer = 0;
        this->Stack.fill(0);
        this->Keypad.clear();
        this->VideoBuffer.fill({});
        this->Size = 0;
        this->TotalTicks = 0;
        this->CurrentOpCode = 0;
        this->LoadedROM = false;
    }

    void CPU::HandleKeyEvent(sf::Event& Event)
    {
        if (Config::Bindings.find(Event.key.code) != Config::Bindings.end())
        {
            this->Keypad[Config::Bindings[Event.key.code]] = (Event.type == sf::Event::KeyPressed);
        }
    }

    void CPU::CycleTimers()
    {
        if (this->DelayTimer > 0)
        {
            this->DelayTimer--;
        }

        if (this->SoundTimer > 0)
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
        const Instruction CurrentInstruction(this->CurrentOpCode);
        switch (CurrentInstruction.Type)
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
                if (this->Registers[CurrentInstruction.x] == CurrentInstruction.kk)
                {
                    this->ProgramCounter += 2;
                }
                this->ProgramCounter += 2;
                break;
            }
            case 0x4000:
            {
                if (this->Registers[CurrentInstruction.x] != CurrentInstruction.kk)
                {
                    this->ProgramCounter += 2;
                }  
                this->ProgramCounter += 2;
                break;
            }
            case 0x5000:
            {
                if (this->Registers[CurrentInstruction.x] == this->Registers[CurrentInstruction.y])
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
                if (this->Registers[CurrentInstruction.x] != this->Registers[CurrentInstruction.y])
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
                const std::uint16_t X = this->Registers[CurrentInstruction.x] % 64;
                const std::uint16_t Y = this->Registers[CurrentInstruction.y] % 32;
                const std::uint16_t Height = CurrentInstruction.n;
                
                this->Registers[0xF] = 0;
                for (std::uint16_t i = 0; i < Height; i++)
                {
                    const std::uint8_t Pixel = this->Memory[I + i];
                    for (std::uint8_t j = 0; j < 8; j++)
                    {
                        if (Pixel & (0x80 >> j))
                        {
                            const std::uint8_t FinalX = (X + j) % 64;
                            const std::uint8_t FinalY = (Y + i) % 32;
                            if (this->VideoBuffer[FinalX][FinalY])
                            {
                                this->Registers[0xF] = 1;
                            }
                            this->VideoBuffer[FinalX][FinalY] ^= 1;
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