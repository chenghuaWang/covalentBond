#include "../pch.hpp"
#include <workflow/WFHttpServer.h>

#include "task/luaEngine.hpp"

int main() {
  //   WFHttpServer server(
  //       [](WFHttpTask* task) { task->get_resp()->append_output_body("<html>Hello World!</html>");
  //       });

  //   if (server.start(8888) == 0) {  // start server on port 8888
  //     getchar();                    // press "Enter" to end.
  //     server.stop();
  //   }
  auto A = luaJitThread();
}
