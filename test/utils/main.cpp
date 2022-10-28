#include "sysStatus.hpp"

int main() {
  cb::utils::sysInfo a;
  cb::utils::getMemoryInfo(a);
  cb::utils::getOsInfo(a);
}