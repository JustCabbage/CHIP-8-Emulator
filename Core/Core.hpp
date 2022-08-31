#pragma once
#include "Emulator/Render.hpp"

namespace Core
{
    inline void RunEmulator(const std::string& ROMPath)
    {
        Emulator::CPU CPU;
        if(ROMPath.size())
        {
            CPU.LoadROM(ROMPath);
        }

        sf::RenderWindow EmulatorWindow(sf::VideoMode({1280, 640}), "CHIP-8 Emulator | github.com/JustCabbage", sf::Style::Close);
        ImGui::SFML::Init(EmulatorWindow);
        ImGui::GetIO().IniFilename = NULL;
        const float TimeIncrement = 1.f / 60.f;
        float TotalTime = 0.f;

        sf::Clock Clock;
        while(EmulatorWindow.isOpen())
        {
            sf::Event event;
            while(EmulatorWindow.pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(event);
                switch(event.type)
                {
                case sf::Event::Closed:
                    EmulatorWindow.close();
                    break;
                case sf::Event::KeyPressed:
                    CPU.HandleKeyEvent(event);
                    break;
                case sf::Event::KeyReleased:
                    CPU.HandleKeyEvent(event);
                    break;
                }
            }
            EmulatorWindow.setFramerateLimit(Config::RefreshRate);
            
            sf::Time DeltaTime = Clock.restart();
            TotalTime += DeltaTime.asSeconds();
            
            ImGui::SFML::Update(EmulatorWindow, DeltaTime);
            Renderer::RenderUI(CPU);
            
            while(TotalTime >= TimeIncrement)
            {
                CPU.CycleTimers();
                TotalTime -= TimeIncrement;
            }

            for(std::int32_t i = 0; i < Config::CyclesPerFrame && CPU.LoadedROM; i++)
                CPU.Cycle();

            EmulatorWindow.clear();
            Renderer::RenderVideoBuffer(EmulatorWindow, CPU);
            ImGui::SFML::Render(EmulatorWindow);
            EmulatorWindow.display();
        }

        ImGui::SFML::Shutdown();
    }

    inline void Initialize(const std::string& ROMPath)
    {
        if(!std::filesystem::exists("roms"))
        {
            std::filesystem::create_directory("roms");
        }
        Core::RunEmulator(ROMPath);
    }
}