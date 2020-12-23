#include "../include/io.h"
#include <vector>
#include <iostream>

int main(int argc, char** argv)
{
    const char* filename = argv[1];
    auto donations = GTD::read_donations(filename, 4);
    for(const auto& donation: donations)
    {
        std::cout << donation << std::endl;
    }

    return 0;
}