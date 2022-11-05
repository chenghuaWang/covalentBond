#include <sol/sol.hpp>
// reference https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_constructors.cpp

#include "task/cbTable.hpp"

int main() {
  cbMySQLCell a(10), b("Hello");
  sol::state lua;
  lua.open_libraries(sol::lib::base);
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
  lua["cell_1"].get_or_create<cbMySQLCell>(&a);
  lua["cell_2"].get_or_create<cbMySQLCell>(&b);
  sol::optional<sol::error> maybe_error = lua.safe_script(R"(
		print(cell_1:asInt());
		print(cell_2:asString());
        cell_3_lua = cbMySQLCell("From lua!!!");
        print(cell_3_lua:asString());
	)",
                                                          sol::script_pass_on_error);

  if (maybe_error) {
    std::cerr << "Something has gone horribly unexpected "
                 "and wrong:\n"
              << maybe_error->what() << std::endl;
    return 1;
  }
  // The only one still need to figure out is the ownership of members created in lua.
}