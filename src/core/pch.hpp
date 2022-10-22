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

#ifdef _WIN32
#define CB_DLL_EXPORT __declspec(dllexport)
#define CB_DLL_IMPORT __declspec(dllimport)
#endif  //! _WIN32
#ifdef __unix__
#define CB_DLL_EXPORT
#define CB_DLL_IMPORT
#endif  //! __unix__

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/os.h>
#include <fmt/color.h>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

#endif  //! __PCH_HPP_