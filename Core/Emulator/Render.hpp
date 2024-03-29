#pragma once
#include <filesystem>
#include <cstdint>
#include <imgui-SFML.h>
#include <imgui.h>
#include "Configuration.hpp"
#include "CPU/CPU.hpp"
#include <MemoryViewer.hpp>

namespace Core::Renderer
{
    inline void RenderVideoBuffer(sf::RenderTarget& Target, const Emulator::CPU& CPU, const Config& Settings)
    {
        for (size_t i = 0; i < CPU.VideoBuffer.size(); i++)
        {
            for (size_t j = 0; j < CPU.VideoBuffer[i].size(); j++)
            {
                if (CPU.VideoBuffer[i][j])
                {
                    sf::RectangleShape Pixel;
                    Pixel.setPosition({static_cast<float>(i * 10), static_cast<float>(j * 10)});
                    Pixel.setSize({10, 10});
                    Pixel.setFillColor(sf::Color(static_cast<uint8_t>(Settings.Color[0] * 255), static_cast<uint8_t>(Settings.Color[1] * 255), static_cast<uint8_t>(Settings.Color[2] * 255)));
                    Target.draw(Pixel);
                }
            }
        }
    }

    inline void RenderUI(Emulator::CPU& CPU, Config& Settings)
    {
        ImGui::StyleColorsDark();
        ImGui::SetNextWindowSize({1280 - 640, 640});
        ImGui::SetNextWindowPos({640, 0});

        ImGui::Begin("Processor Information", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        {
            ImGui::BeginTabBar("#ProcessorInfoTabs");
            {
                if (ImGui::BeginTabItem("Statistics"))
                {
                    ImGui::BeginChild("#StatisticsChild", ImVec2(625, 0), true);
                    ImGui::Columns(2, 0, false);
                    ImGui::Text("Registers");
                    ImGui::NextColumn();
                    ImGui::Text("Stack");
                    ImGui::Separator();
                    ImGui::NextColumn();

                    for (int i = 0; i < CPU.Registers.size(); i++)
                    {
                        ImGui::Text("V%d - %X", i, CPU.Registers[i]);
                    }

                    ImGui::NextColumn();

                    for (int i = 0; i < CPU.Stack.size(); i++)
                    {
                        ImGui::Text("#%d - %X", i, CPU.Stack[i]);
                    }

                    ImGui::Columns(1);
                    ImGui::Separator();

                    ImGui::Text("Framerate - %.2f", ImGui::GetIO().Framerate);
                    ImGui::Text("Current Instruction - %X", CPU.CurrentOpCode);
                    ImGui::Text("Total Cycles - %X", CPU.TotalTicks);
                    ImGui::Text("Program Counter - %X", CPU.ProgramCounter);
                    ImGui::Text("Stack Pointer - %d", CPU.StackPointer);
                    ImGui::Text("Delay Timer - %d", CPU.DelayTimer);
                    ImGui::Text("Sound Timer - %d", CPU.SoundTimer);
                    ImGui::Text("I - %X", CPU.I);

                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                
                if (ImGui::BeginTabItem("Memory View"))
                {
                    static MemoryEditor MemoryViewer;
                    MemoryViewer.DrawContents(CPU.Memory.data(), CPU.Memory.size());
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        ImGui::SetNextWindowSize({640, 640 - 320});
        ImGui::SetNextWindowPos({0, 320});
        
        ImGui::Begin("Configuration", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        {
            ImGui::BeginTabBar("#ConfigurationBar");
            {
                if (ImGui::BeginTabItem("Settings"))
                {
                    ImGui::SliderInt("Cycles Per Frame", &Settings.CyclesPerFrame, 0, 1000);
                    ImGui::SliderInt("Refresh Rate", &Settings.RefreshRate, 1, 1000);
                    ImGui::ColorEdit3("Color", Settings.Color.data());
                    if (ImGui::Button("Reset"))
                    {
                        CPU.Reset();
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("ROMs"))
                {
                    for (const auto& Entry : std::filesystem::recursive_directory_iterator("roms"))
                    {
                        if(Entry.path().extension() == ".ch8")
                        {
                            if (ImGui::Button(Entry.path().string().c_str()))
                            {
                                CPU.LoadROM(Entry.path().string());
                            }
                        }
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }
}