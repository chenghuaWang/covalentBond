#include "liveThread.hpp"
#include "task/luaEngine.hpp"
int main() {
  // frontThread t(8888, "cb-server-test", "test cb server");
  // t.exec();
  luaJitThread l;
  l.loadScriptFromFile("/home/wang/covalentBond/scripts/builtinOP/op_direct_combine.lua");
  auto res = l.execMain();
}
