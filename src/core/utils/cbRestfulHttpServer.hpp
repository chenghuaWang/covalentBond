/**
 * @file cbRestfulHttpServer.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief A restful api like http server for programming post and get
 * conveniently.
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __UTILS_CB_RESTFUL_HTTP_SERVER_HPP_
#define __UTILS_CB_RESTFUL_HTTP_SERVER_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../pch.hpp"

#include <workflow/WFFacilities.h>
#include <wfrest/HttpServer.h>

using namespace wfrest;

namespace cb {
namespace utils {

class cbRestfulHttpServer {
 public:
  ~cbRestfulHttpServer();
  cbRestfulHttpServer(unsigned short port);

  void execMain();

  void stopMain();

  HttpServer& operator()();

 private:
  HttpServer* m_svr;
  unsigned short m_port;
};

}  // namespace utils
}  // namespace cb

#endif  //! __UTILS_CB_RESTFUL_HTTP_SERVER_HPP_