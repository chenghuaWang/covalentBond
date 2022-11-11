#include "task/luaEngine.hpp"

int main() {
  luaJitThread t;
  t.execMain("function printMe(rhs)"
             "  print(rhs);"
             "end");
  sol::function func = t()["printMe"];
  func("Hello from cpp, but execute by lua\n");
}