#include "../pch.hpp"
int main() { fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "Hello, {}!\n", "world"); }