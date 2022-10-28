#include "liveThread.hpp"

int main() {
  frontThread t(8888, "cb-server-test", "test cb server");
  t.exec();
}
