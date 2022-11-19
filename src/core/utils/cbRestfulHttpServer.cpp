#include "cbRestfulHttpServer.hpp"

namespace cb {
namespace utils {

cbRestfulHttpServer::~cbRestfulHttpServer() { delete m_svr; }

cbRestfulHttpServer::cbRestfulHttpServer(unsigned short port)
    : m_svr(new HttpServer()), m_port(port) {}

void cbRestfulHttpServer::execMain() {
  if (m_svr->start(m_port) == 0) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "[ info ] Backend listener start at port {}.\n", m_port);
    m_svr->list_routes();
  } else {
    perror("Cannot start server");
    exit(1);
  }
}

void cbRestfulHttpServer::stopMain() { m_svr->stop(); }

HttpServer& cbRestfulHttpServer::operator()() { return *m_svr; }

}  // namespace utils
}  // namespace cb