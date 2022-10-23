/**
 * @file sysStatus.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief utils/sysStatus.hpp ... To get the infomation of system. For
 * both windows and unix. Those informations will also be serialized to
 * binary or json files for persistence.
 * @version 0.1
 * @date 2022-10-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __UTILS_SYS_STATUS_HPP_
#define __UTILS_SYS_STATUS_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../pch.hpp"

#define __DESCRIBE_CHAR_MAX_NUMS 64

namespace cb {
namespace utils {

struct sysInfo {
  int32_t m_memAll;
  int32_t m_memUsed;
  int32_t m_cpuCores;
  int32_t m_cpuHz;
  char m_cpuType[__DESCRIBE_CHAR_MAX_NUMS];
  char m_netCardType[__DESCRIBE_CHAR_MAX_NUMS];
  // TODO get IP/MAC/etc.
};

#if defined(_WIN32)
#elif defined(__unix__)

#endif

#undef __DESCRIBE_CHAR_MAX_NUMS

}  // namespace utils
}  // namespace cb

#endif  //! __UTILS_SYS_STATUS_HPP_