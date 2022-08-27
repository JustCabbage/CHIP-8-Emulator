#pragma once
#include <imgui-SFML.h>
#include <imgui.h>
#include "Configuration.hpp"
#include "CPU.hpp"

namespace Core::Renderer
{
    inline void RenderVideoBuffer(sf::RenderTarget& Target, Emulator::CPU& CPU)
    {
        for(int i = 0; i < CPU.VideoBuffer.size(); i++)
        {
            for(int j = 0; j < CPU.VideoBuffer[i].size(); j++)
            {
                if(CPU.VideoBuffer[i][j])
                {
                    sf::RectangleShape Pixel;
                    Pixel.setPosition({static_cast<float>(i * 10), static_cast<float>(j * 10)});
                    Pixel.setSize({10, 10});
                    Pixel.setFillColor(sf::Color::Blue);
                    Target.draw(Pixel);
                }
            }
        }
    }

    inline void RenderUI(Emulator::CPU& CPU)
    {
        ImGui::SetNextWindowSize({1280 - 640, 640});
        ImGui::SetNextWindowPos({640, 0});

        ImGui::Begin("Processor Information", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        {
            ImGui::BeginTabBar("#ProcessorInfoTabs");
            {
                ImGui::BeginTabItem("Registers");
                {
                    for(int i = 0; i < CPU.Registers.size(); i++)
                    {
                        ImGui::Text("V%X - %X", i, CPU.Registers[i]);
                    }
                    ImGui::Text("Current Instruction - %X", CPU.CurrentOpCode);
                    ImGui::Text("Total Ticks - %d", CPU.TotalTicks);
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        ImGui::SetNextWindowSize({640, 640 - 320});
        ImGui::SetNextWindowPos({0, 320});
        
        ImGui::Begin("Configuration", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        {
            ImGui::SliderInt("Cycles Per Frame", &Config::CyclesPerFrame, 0, 1000);
        }
        ImGui::End();
    }
}