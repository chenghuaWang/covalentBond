#ifndef __SERVER_LIVE_THREAD_HPP_
#define __SERVER_LIVE_THREAD_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "sysStatus.hpp"
#include "trivial/cfgIO.hpp"

class frontThread {
 public:
  frontThread(unsigned short httpServerPort, const std::string& name, const std::string& describe)
      : m_httpServerPort(httpServerPort) {
    // init all infos
    cb::utils::getMemoryInfo(m_sysInfo);
    cb::utils::getOsInfo(m_sysInfo);
    strcpy(m_cacheFile.osInfo, m_sysInfo.m_osInfo);
    strcpy(m_cacheFile.name, name.c_str());
    strcpy(m_cacheFile.describe, describe.c_str());
    // create files. store lock. creat html if not exists
    trivial::writeCacheFileBinary(m_cacheFile);
    if (access("./index.html", F_OK)) {
      std::ofstream fp("./index.html");
      fp << "<html>Hello World!</html>";
      fp.close();
    }
  }

  virtual void exec();

 private:
  unsigned short m_httpServerPort = 8888;
  cb::utils::sysInfo m_sysInfo;
  trivial::cacheFile m_cacheFile;
};

class daemonThread {};

#endif  //! __SERVER_LIVE_THREAD_HPP_