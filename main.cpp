#include "Core/Core.hpp"

int main(int argc, char** argv)
{
    const std::string Path = argc > 1 ? argv[1] : "";
    Core::Initialize(Path);
    return EXIT_SUCCESS;
}