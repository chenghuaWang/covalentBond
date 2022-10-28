/**
 * @file cfgIO.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief Manage all config file. Write to Binary.
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __SERVER_CFG_IO_HPP_
#define __SERVER_CFG_IO_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../../pch.hpp"

#define CFG_CHAR_NUMS 512
#define CFG_CHAR_NUMS_DOUBLE 1024

namespace trivial {

#pragma pack(push, 1)
struct cacheFile {
  uint32_t magicNumber = 0x20121902;
  char name[CFG_CHAR_NUMS];
  char describe[CFG_CHAR_NUMS];
  char osInfo[CFG_CHAR_NUMS_DOUBLE];
  // others.
};
#pragma pack(pop)

bool readCacheFileBinary(cacheFile& cf);
bool writeCacheFileBinary(const cacheFile& cf);

}  // namespace trivial

#endif  //! __SERVER_CFG_IO_HPP_