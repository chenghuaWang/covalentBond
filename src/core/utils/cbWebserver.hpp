/**
 * @file cbWebserver.hpp
 * @author caibo feng (3418552929@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-10-30
 * @note
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __UTILS_CB_WEB_SERVER_HPP_
#define __UTILS_CB_WEB_SERVER_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../pch.hpp"
#include <signal.h>
#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>

typedef std::function<void(WFHttpTask*, char*)> http_func_handle;

namespace cb {
namespace utils {

/**
 * @brief
 *
 */
class cbWebserver {
 public:
  cbWebserver() = default;

  /**
   * @brief Construct a new cb Webserver object
   *
   * @param port
   * @param root
   */
  cbWebserver(const int32_t& port, const std::string& root);
  ~cbWebserver();

  /**
   * @brief
   *
   */
  void execMain();

  /**
   * @brief
   *
   */
  void stopMain();

  /**
   * @brief Get the Server object
   *
   * @return WFHttpServer*
   */
  WFHttpServer* getServer();

 private:
  static void signal_kill(int signo);

  int32_t m_port;
  std::string m_root;
  WFHttpServer* m_server;
};

}  // namespace utils
}  // namespace cb

#endif  //! __UTILS_CB_WEB_SERVER_HPP_