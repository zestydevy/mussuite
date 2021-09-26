#include <cstdint>
#include <iostream>
#include <fmt/core.h>

// -------------------------------------------------------------------------- //

constexpr char sUsageStr[] =
R"(
  Usage:
    mussuite [options] <input>
  Options:
    -h, --help              show this help
)";

// -------------------------------------------------------------------------- //

int main(int argc, char * argv[])
{
    fmt::print(sUsageStr);
    return argc;
}
