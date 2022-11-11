#include "cbWebserver.hpp"

int main(int argc, char* argv[]) {
  const char* root = ".";
  cb::utils::cbWebserver server(8088, root);
  server.execMain();
  return 0;
}
