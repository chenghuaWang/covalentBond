#include "cbLRUCache.hpp"

int main(int argc, char* argv[]) {
  cb::cbLRUCache<int32_t> test_cache(32);
  int32_t values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  std::vector<std::string> keys;
  std::string buf;
  char ke[] = "key1";
  int32_t i = 0;
  int32_t ind = 14;
  int32_t* j;
  while (1) {
    if (i > 72) { i = 0; }
    if (ind < 0) { ind = 14; }
    ke[3] = static_cast<char>(i) + '0';
    fmt::print(ke);
    std::cout << ':' << values[ind] << std::endl;
    // key 值 相同则更新value
    buf = ke;
    test_cache.push(buf, values[ind]);
    if (!(i % 10)) {
      j = test_cache.get("key0");
      std::cout << "key0 : value: " << *j << std::endl;
    }
    i++;
    ind--;
  }
  return 0;
}
