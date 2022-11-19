/**
 * @file cbWebserver.cpp
 * @author caibo feng (3418552929@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-10-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "cbWebserver.hpp"

namespace cb {
namespace utils {

cbWebserver::~cbWebserver() { delete m_server; }

cbWebserver::cbWebserver(const int32_t& port, const std::string& root)
    : m_port(port), m_root(root) {
  m_server = new WFHttpServer([=](WFHttpTask* server_task) -> void {
    protocol::HttpRequest* req = server_task->get_req();
    protocol::HttpResponse* resp = server_task->get_resp();

    /* Set response message body. */
    /* Set status line if you like. */
    resp->set_http_version("HTTP/1.1");
    resp->set_status_code("200");
    resp->set_reason_phrase("OK");

    const char* buf = req->get_request_uri();
    const char* p = buf;
    // printf("Request-UTI: %s\n", buf);
    while (*buf && *buf != '/') buf++;
    while (*p) p++;

    const char* exten = p - 1;
    while (*exten && *exten != '.') exten--;
    exten++;
    std::string file_name_extension(exten, p - exten);
    if (file_name_extension == "js") file_name_extension = "javascript";
    std::string content_type = "text/" + file_name_extension;
    if (file_name_extension == "svg") content_type = "image/" + file_name_extension + "+xml";

    std::string abs_path(buf, p - buf);
    abs_path = m_root + abs_path;

    resp->set_header_pair("Content-Type", content_type);

    int fd = open(abs_path.c_str(), O_RDONLY);
    if (fd >= 0) {
      size_t size = lseek(fd, 0, SEEK_END);
      void* buf = malloc(size); /* As an example, assert(buf != NULL); */
      WFFileIOTask* pread_task;
      pread_task = WFTaskFactory::create_pread_task(fd, buf, size, 0, [=](WFFileIOTask* task) {
        FileIOArgs* args = task->get_args();
        long ret = task->get_retval();
        // std::cout << ret << std::endl;
        protocol::HttpResponse* resp = (protocol::HttpResponse*)task->user_data;

        /* close fd only when you created File IO task with **fd** interface. */
        close(args->fd);
        if (ret < 0) {
          resp->set_status_code("503");
          resp->append_output_body("<html>503 Internal Server Error.</html>");
        } else /* Use '_nocopy' carefully. */
          resp->append_output_body(args->buf, ret);
      });

      /* To implement a more complicated server, please use series' context
       * instead of tasks' user_data to pass/store internal data. */
      pread_task->user_data = resp; /* pass resp pointer to pread task. */
      server_task->user_data = buf; /* to free() in callback() */
      server_task->set_callback([](WFHttpTask* t) { free(t->user_data); });
      series_of(server_task)->push_back(pread_task);
    } else {
      resp->set_status_code("404");
      resp->append_output_body("<html>404 Not Found.</html>");
    }
  });
}

void cbWebserver::execMain() {
  fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
             "[ info ] Get the ./index.html file. Start HTTP server at {}{}/index.html\n",
             "http://localhost:", m_port);
  fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
             "[ info ] Press ^C to stop server.\n");

  signal(SIGTERM, signal_kill);
  if (m_server->start(m_port) == 0) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "[ info ] server successfully started.\n");
  } else {
    perror("Cannot start server");
    exit(1);
  }
}

void cbWebserver::stopMain() { m_server->stop(); }

WFHttpServer* cbWebserver::getServer() { return m_server; }

void cbWebserver::signal_kill(int signo) {}

}  // namespace utils
}  // namespace cb