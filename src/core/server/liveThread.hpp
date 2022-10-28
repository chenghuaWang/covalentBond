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
  frontThread(const std::string& name, const std::string& describe) {
    // init all infos. created all directory, files if not exists.
    cb::utils::getMemoryInfo(m_sysInfo);
    cb::utils::getOsInfo(m_sysInfo);
    strcpy(m_sysInfo.m_osInfo, m_cacheFile.osInfo);
  }

 private:
  cb::utils::sysInfo m_sysInfo;
  trivial::cacheFile m_cacheFile;
};

#endif  //! __SERVER_LIVE_THREAD_HPP_