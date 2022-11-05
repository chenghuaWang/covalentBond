#include <sol/sol.hpp>

#include <string>
#include <cstdlib>

// reference https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_constructors.cpp

class test_class {
 public:
  int asInt() { return std::atoi((char*)data); }
  void setAsInt(int a) {
    std::string tmp = std::to_string(a);
    len = tmp.length();
    data = (void*)std::move(tmp.c_str());
    std::cout << tmp << std::endl;
  }
  size_t len;
  void* data;
};

int main() {
  test_class a;
  a.setAsInt(32);
  std::cout << a.asInt();
}
