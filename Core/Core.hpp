#pragma once
#include "Emulator/Render.hpp"

namespace Core
{
    inline void Initialize(const std::string& RomPath)
    {
        Emulator::CPU CPU(RomPath);
        sf::RenderWindow EmulatorWindow(sf::VideoMode({1280, 640}), "CHIP-8 Emulator | github.com/JustCabbage", sf::Style::Close);
        EmulatorWindow.setFramerateLimit(60);
        ImGui::SFML::Init(EmulatorWindow);
        ImGui::GetIO().IniFilename = NULL;

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
            
            ImGui::SFML::Update(EmulatorWindow, Clock.restart());
            Renderer::RenderUI(CPU);
            
            CPU.CycleTimers();

            for(std::int32_t i = 0; i < Config::CyclesPerFrame; i++)
                CPU.Cycle();

            EmulatorWindow.clear();
            Renderer::RenderVideoBuffer(EmulatorWindow, CPU);
            ImGui::SFML::Render(EmulatorWindow);
            EmulatorWindow.display();
        }

        ImGui::SFML::Shutdown();
    }
}