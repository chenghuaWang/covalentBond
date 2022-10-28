#include "liveThread.hpp"
#include <workflow/WFHttpServer.h>

void frontThread::exec() {
  // start execute the website.
  std::ifstream fp("./index.html");
  std::string __htmlIndexBody((std::istreambuf_iterator<char>(fp)),
                              std::istreambuf_iterator<char>());
  fp.close();
  fmt::print(fg(fmt::color::floral_white) | bg(fmt::color::slate_gray) | fmt::emphasis::underline,
             "--- OS info ---\n");
  fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic, "{}\n", m_sysInfo.m_osInfo);
  fmt::print("[ info ] Get the ./index.html file. Start HTTP server at {}{}\n",
             "http://localhost:", m_httpServerPort);
  fmt::print("[ info ] Press ^C to stop server.\n");
  WFHttpServer server(
      [=](WFHttpTask* task) { task->get_resp()->append_output_body(__htmlIndexBody.c_str()); });
  // Fork a daemon thread here and do other things.
  // And in the scope below, need to reduce the thread forked.
  if (server.start(m_httpServerPort) == 0) {
    pause();
    server.stop();
  }
}