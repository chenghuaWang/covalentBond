/**
 * @file pch.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The precompiled head.
 * @version 0.1
 * @date 2022-10-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __PCH_HPP_
#define __PCH_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <functional>

#ifdef _WIN32
#define CB_DLL_EXPORT __declspec(dllexport)
#define CB_DLL_IMPORT __declspec(dllimport)
#endif  //! _WIN32
#ifdef __unix__
#define CB_DLL_EXPORT
#define CB_DLL_IMPORT
#endif  //! __unix__

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/os.h>
#include <fmt/color.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

// log system used is Google log
#define CB_GLOG 1

#if CB_GLOG == 0
enum class _log_type { all = 0, debug, warn, info, err };

class _log_stream {
 public:
  static std::ostream& make_stream(const char* file, int line, const _log_type& t) {
    switch (t) {
      case _log_type::warn: {
        std::cout << "[Warn] [" << file << "(" << line << ", 0)"
                  << "] ";
        return std::cout;
      }
      case _log_type::err: {
        std::cerr << "[Err] [" << file << "(" << line << ", 0)"
                  << "] ";
        return std::cerr;
      }
      case _log_type::debug: {
        std::cout << "[Debug] [" << file << "(" << line << ", 0)"
                  << "] ";
        return std::cout;
      }
      default: {
        std::cout << "[Info] [" << file << "(" << line << ", 0)"
                  << "] ";
        return std::cout;
      }
    }
  }
};

#define LOG_INFO _log_stream::make_stream(__FILE__, __LINE__, _log_type::info)
#define LOG_ERR _log_stream::make_stream(__FILE__, __LINE__, _log_type::err)
#define LOG_WARN _log_stream::make_stream(__FILE__, __LINE__, _log_type::warn)
#define LOG_DEBUG _log_stream::make_stream(__FILE__, __LINE__, _log_type::debug)
#define LOG _log_stream::make_stream(__FILE__, __LINE__, _log_type::all)

template<typename T>
inline std::ostream& operator,(std::ostream& out, const T& t) {
  out << t;
  return out;
}

inline std::ostream& operator,(std::ostream& out, std::ostream& (*f)(std::ostream&)) {
  out << f;
  return out;
}

/*!
 *@brief CHECK if the condition is True.
 */
#define LOG_CHECK(x, ...)                            \
  if (!(x)) {                                        \
    LOG_ERR << "Failed when testing " << #x << "\n"; \
    std::cerr, __VA_ARGS__, std::endl;               \
    std::exit(1);                                    \
  }

#define CHECK_LOWER_THAN(x, y, ...) LOG_CHECK((x) < (y), __VA_ARGS__)
#define CHECK_GREATER_THAN(x, y, ...) LOG_CHECK((x) > (y), __VA_ARGS__)
#define CHECK_EQUAL(x, y, ...) LOG_CHECK((x) == (y), __VA_ARGS__)
#define CHECK_LOWER_EQUAL(x, y, ...) LOG_CHECK((x) <= (y), __VA_ARGS__)
#define CHECK_GREATER_EQUAL(x, y, ...) LOG_CHECK((x) >= (y), __VA_ARGS__)
#define CHECK_NOT_EQUAL(x, y, ...) LOG_CHECK((x) != (y), __VA_ARGS__)
#define CHECK_NULL(x, ...) LOG_CHECK((x) != NULL, __VA_ARGS__)

#else
#include <glog/logging.h>

#define LOG_INFO LOG(INFO)
#define LOG_ERR LOG(ERROR)
#define LOG_WARN LOG(WARN)
#define LOG_FATAL LOG(FATAL)

#define LOG_CHECK(x, ...)                            \
  if (!(x)) {                                        \
    LOG_ERR << "Failed when testing " << #x << "\n"; \
    std::cerr, __VA_ARGS__, std::endl;               \
    std::exit(1);                                    \
  }

#define CHECK_LOWER_THAN(x, y, ...) LOG_CHECK((x) < (y), __VA_ARGS__)
#define CHECK_GREATER_THAN(x, y, ...) LOG_CHECK((x) > (y), __VA_ARGS__)
#define CHECK_EQUAL(x, y, ...) LOG_CHECK((x) == (y), __VA_ARGS__)
#define CHECK_LOWER_EQUAL(x, y, ...) LOG_CHECK((x) <= (y), __VA_ARGS__)
#define CHECK_GREATER_EQUAL(x, y, ...) LOG_CHECK((x) >= (y), __VA_ARGS__)
#define CHECK_NOT_EQUAL(x, y, ...) LOG_CHECK((x) != (y), __VA_ARGS__)
#define CHECK_NULL(x, ...) LOG_CHECK((x) != NULL, __VA_ARGS__)

#endif  //! CB_GLOG == 0

/**
 * @brief    To align the data.
 * @details  x normally set to 4, 8, 16. This attribute will make
 * data aligned to x.
 * For example, you can use this macro like this:
 * float foo_1[10] CB_ALIGNED(4);
 * int32_t foo_2[10] CB_ALIGNED(8);
 */
#define CB_ALIGNED(x) __attribute__((aligned(x)))

// inline symbol for template code used in both gpu and cpu
#ifdef CB_INLINE
#error "You can not predefine CB_INLINE. It's used in this lib."
#endif
#define CB_FORCE_INLINE inline __attribute__((always_inline))
#define CB_INLINE_CPU CB_FORCE_INLINE
#define CB_INLINE_NORMAL inline

// defined the const exp
#define CB_CONSTEXPR constexpr

#define CB_IS_NAN(func) std::isnan(func)
#define CB_IS_INF(func) std::isinf(func)

#define NO_TYPE_PTR void*

#define CB_SHAPE_ERROR_EXIT 1
#define CB_TYPE_ERROR_EXIT 2
#define CB_MEM_ERROR_EXIT 3
#define CB_OTHER_ERROR_EXIT 4

#define CB_USE_SOL true

#define MAKE_SHARED(x) std::make_shared<x>
#define SHARED_PTR(x) std::shared_ptr<x>

#endif  //! __PCH_HPP_