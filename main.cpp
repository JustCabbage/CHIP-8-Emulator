#include "Core/Core.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cout << "Usage - CHIP-8-Emulator <rom>";
        return 0;
    }
    Core::Initialize(argv[1]);
    return 0;
}