/**
 * @file luaEngine.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief  A lua engine for execute all c++ embedding in struct.
 * A warper of lua JIT engine.
 * @version 0.1
 * @date 2022-10-27
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __SERVER_LUA_ENGINE_HPP_
#define __SERVER_LUA_ENGINE_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../../pch.hpp"

#if CB_USE_SOL == false
//! make compiled in C ABI, and all libs is installed to system path.
extern "C" {
#include <lua.h>
#include <luajit.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <any>
#else
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#endif

#include "cbTable.hpp"
#include "cbOperator.hpp"

void bindAllFuncByDefault(sol::state* handle);

class luaJitThread {
 public:
  ~luaJitThread();
  luaJitThread();

  void execMain(const std::string& script);

  sol::state& operator()();

 private:
  sol::state* m_luaHandle;
};

#endif  //! __SERVER_LUA_ENGINE_HPP_
