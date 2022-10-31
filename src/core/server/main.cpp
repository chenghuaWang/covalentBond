#include <sol/forward.hpp>
#include "liveThread.hpp"
#include "task/luaEngine.hpp"

class my_class {
 public:
  my_class(int m) : p(m) {}
  std::variant<int32_t, std::string> p = 10;
  std::string a = "name";
  int32_t asInt() { return std::get<int32_t>(p); }
  void setAsInt(sol::object a) { p = a.as<int32_t>(); }
};

int main() {
  // frontThread t(8888, "cb-server-test", "test cb server");
  // t.exec();
  // std::vector<std::vector<__baseObj>> b;
  // std::vector<__baseObj> a;
  // luaJitThread l;
  // l.loadScriptFromFile("/home/wang/covalentBond/scripts/builtinOP/op_direct_combine.lua");
  // l.execMain(3, b, a);
  auto class_ptr = new my_class(10);
  sol::state lua;
  lua.open_libraries();
  auto bark = lua["cpp"].get_or_create<sol::table>();
  bark.new_usertype<my_class>("my_class", "asInt", &my_class::asInt, "setAsInt",
                              &my_class::setAsInt);
  lua["obj"].get_or_create<sol::usertype<my_class>>(class_ptr);
  lua.script("obj:setAsInt(obj:asInt() + 1);");
  std::cout << class_ptr->asInt();
  delete class_ptr;
}
