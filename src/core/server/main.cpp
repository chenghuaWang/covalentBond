// #include <sol/sol.hpp>
#include "trivial/cbVirtualDevice.hpp"

// class my_class {
//  public:
//   my_class(int m) : p(m) {}
//   std::variant<int32_t, std::string> p = 10;
//   std::string a = "name";
//   int32_t asInt() { return std::get<int32_t>(p); }
//   void setAsInt(sol::object a) { p = a.as<int32_t>(); }
// };

int main() {
  // frontThread t(8888, "cb-server-test", "test cb server");
  // t.exec();
  // std::vector<std::vector<__baseObj>> b;
  // std::vector<__baseObj> a;
  // luaJitThread l;
  // l.loadScriptFromFile("/home/wang/covalentBond/scripts/builtinOP/op_direct_combine.lua");
  // l.execMain(3, b, a);

  // lua
  // auto class_ptr = new my_class(10);
  // auto class_ptr_1 = new my_class(0);
  // sol::state lua;
  // lua.open_libraries();
  // auto bark = lua["cpp"].get_or_create<sol::table>();
  // bark.new_usertype<my_class>("my_class", "asInt", &my_class::asInt, "setAsInt",
  //                             &my_class::setAsInt);
  // lua["obj_in"].get_or_create<sol::usertype<my_class>>(class_ptr);
  // lua["obj_out"].get_or_create<sol::usertype<my_class>>(class_ptr_1);
  // lua.script("obj_in:setAsInt(obj_in:asInt() + 1);");
  // lua.script("obj_out = obj_in");
  // std::cout << class_ptr->asInt() << std::endl;
  // std::cout << class_ptr_1->asInt() << std::endl;
  // delete class_ptr;
  // delete class_ptr_1;

  // Mysql Virtual device
}
