#include "task/luaEngine.hpp"

int main() {
  luaJitThread t;
  t.execMain("a = cb.KVCell(12.0);"
             "print(a:asFloat());"
             "b = cb.KVCell(\"Hello from lua.\");"
             "print(b:asString());"
             "c = cb.F.makeShapeFull(1, 1);");
  cbMySQLCell* m = t()["a"];
  std::cout << m->asFloat() << std::endl;
  t.execMain("a:setFloat(0.0001);");
  std::cout << m->asFloat() << std::endl;
}