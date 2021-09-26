#include <cstdint>
#include <iostream>

// -------------------------------------------------------------------------- //

static char const sUsageStr[] =
R"(
  Usage:
    mussuite [options] <input>
  Options:
    -h, --help              show this help
)";

// -------------------------------------------------------------------------- //

int main(int32_t argc, char8_t * argv[])
{
    std::cout << sUsageStr << std::endl;
    return argc;
}