#include "cfgIO.hpp"

namespace trivial {

bool writeCacheFileBinary(const cacheFile& cf) {
  std::ofstream fp(".cbServer", std::ios::binary);
  if (fp.good()) {
    fp.write(reinterpret_cast<const char*>(&cf), sizeof(cf));
    return true;
  }
  return false;
}

}  // namespace trivial