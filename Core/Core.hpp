#pragma once
#include "Emulator/CPU.hpp"
#include <SFML/Graphics.hpp>

namespace Core
{
    void Initialize(const std::string& ROMPath)
    {
        Emulator::CPU CPU(ROMPath);
        sf::RenderWindow EmulatorWindow(sf::VideoMode({640, 320}), "CHIP-8 Emulator | github.com/JustCabbage", sf::Style::Close);
        
        while(EmulatorWindow.isOpen())
        {
            sf::Event e;
            while(EmulatorWindow.pollEvent(e))
            {
                switch(e.type)
                {
                case sf::Event::Closed:
                    EmulatorWindow.close();
                    break;
                }
            }

            CPU.Cycle();

            EmulatorWindow.clear();
            for(int i = 0; i < CPU.VideoBuffer.size(); i++)
            {
                for(int j = 0; j < CPU.VideoBuffer[i].size(); j++)
                {
                    if(CPU.VideoBuffer[i][j])
                    {
                        sf::RectangleShape Pixel;
                        Pixel.setPosition({(float)(i * 10), (float)(j * 10)});
                        Pixel.setSize({10,10});
                        Pixel.setFillColor(sf::Color::Blue);
                        EmulatorWindow.draw(Pixel);
                    }
                }
            }
            EmulatorWindow.display();
        }
    }
}