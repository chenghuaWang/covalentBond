#include <sol/sol.hpp>
// reference https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_constructors.cpp

#include "task/cbTable.hpp"

struct my_class {
  my_class() : data(2) {}
  void setAt(int i, cbMySQLCell* a) { data[i] = a; }
  std::vector<cbMySQLCell*> data;
};

int main() {
  cbMySQLCell a(10), b("Hello");
  cbMySQLCell* c = new cbMySQLCell("lua aaaaaa !!!");
  my_class container;
  sol::state lua;
  lua.open_libraries(sol::lib::base);
  lua.new_usertype<my_class>("my_class", "setAt", &my_class::setAt);
  lua.new_usertype<cbMySQLCell>(
      "cbMySQLCell", sol::meta_function::construct,
      sol::factories([](const double& d) { return std::make_shared<cbMySQLCell>(d); },
                     [](const int& d) { return std::make_shared<cbMySQLCell>(d); },
                     [](const std::string& d) { return std::make_shared<cbMySQLCell>(d); }),
      sol::call_constructor,
      sol::factories([](const double& d) { return std::make_shared<cbMySQLCell>(d); },
                     [](const int& d) { return std::make_shared<cbMySQLCell>(d); },
                     [](const std::string& d) { return std::make_shared<cbMySQLCell>(d); }),
      "asString", &cbMySQLCell::asString, "asInt", &cbMySQLCell::asInt, "asDouble",
      &cbMySQLCell::asDouble, "setString", &cbMySQLCell::setString, "setInt", &cbMySQLCell::setInt,
      "setDouble", &cbMySQLCell::setDouble);
  lua["container"].get_or_create<my_class>(&container);
  lua["cell_1"].get_or_create<cbMySQLCell>(&a);
  lua["cell_2"].get_or_create<cbMySQLCell>(&b);
  lua["create"] = [=]() -> cbMySQLCell* { return c; };
  sol::optional<sol::error> maybe_error = lua.safe_script(R"(
		print(cell_1:asInt());
		print(cell_2:asString());
        cell_3_lua = cbMySQLCell("From lua!!!");
        print(cell_3_lua:asString());
        container:setAt(0, cell_2);
	)",
                                                          sol::script_pass_on_error);

  if (maybe_error) {
    std::cerr << "Something has gone horribly unexpected "
                 "and wrong:\n"
              << maybe_error->what() << std::endl;
    return 1;
  }
  // The only one still need to figure out is the ownership of members created in lua.
  std::cout << container.data[0]->asString() << std::endl;
}