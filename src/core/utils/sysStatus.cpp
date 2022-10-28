#include "sysStatus.hpp"

namespace cb {
namespace utils {

void getMemoryInfo(sysInfo& si) {
  std::ifstream fp("/proc/meminfo");
  if (!fp.good()) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "[ Error ] Failed to open memInfo");
  }
  std::string __buffer;
  fp >> __buffer >> si.m_memAll >> __buffer;
  fp >> __buffer >> si.m_memFree >> __buffer;
  fp.close();
}

void getOsInfo(sysInfo& si) {
  FILE* fp = fopen("/proc/version", "r");
  if (NULL == fp) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "[ Error ] Failed to open cpuInfo");
  }
  fgets(si.m_osInfo, sizeof(si.m_osInfo) - 1, fp);
  fclose(fp);
}

}  // namespace utils
}  // namespace cb