#pragma once
#include "Emulator/Render.hpp"

namespace Core
{
    void Initialize(const std::string& ROMPath)
    {
        Emulator::CPU CPU(ROMPath);
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
                }
            }
            
            ImGui::SFML::Update(EmulatorWindow, Clock.restart());
            Renderer::RenderUI(CPU);
            
            CPU.Cycle();

            EmulatorWindow.clear();
            Renderer::RenderVideoBuffer(EmulatorWindow, CPU);
            ImGui::SFML::Render(EmulatorWindow);
            EmulatorWindow.display();
        }

        ImGui::SFML::Shutdown();
    }
}