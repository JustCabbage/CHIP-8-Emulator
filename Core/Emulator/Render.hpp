#pragma once
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "CPU.hpp"

namespace Core::Renderer
{
    void RenderVideoBuffer(sf::RenderTarget& Target, Emulator::CPU& CPU)
    {
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
                    Target.draw(Pixel);
                }
            }
        }
    }

    void RenderUI(Emulator::CPU& CPU)
    {
        ImGui::SetNextWindowSize({1280 - 640, 720});
        ImGui::SetNextWindowPos({640, 0});
        if(ImGui::Begin("Processor Information", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            if(ImGui::BeginTabBar("#ProcessorInfoTabs"))
            {
                ImGui::BeginTabItem("Registers");
                std::array<std::uint8_t, 16> Registers = CPU.GetRegisters();
                for(int i = 0; i < Registers.size(); i++)
                {
                    ImGui::Text("V%X - %X", i, Registers[i]);
                }
                ImGui::Text("Current Instruction - %X", CPU.GetCurrentOpCode());
                ImGui::EndTabItem();

                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        ImGui::SetNextWindowSize({640, 720 - 320});
        ImGui::SetNextWindowPos({0, 320});
        if(ImGui::Begin("Configuration", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::End();
        }
    }
}