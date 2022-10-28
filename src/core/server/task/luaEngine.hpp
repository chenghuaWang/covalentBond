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
#else
#include <sol/sol.hpp>
#endif

extern "C" {
// reference https://github.com/ThePhD/sol2
// reference https://aiplay.github.io/2016/08/09/2016-08-09-sol2-tutorail/
class luaJitThread {
 public:
  luaJitThread() : m_lua_handle(sol::state()) { m_lua_handle.open_libraries(); }

  void loadScript2Mem(const std::string& rhs);
  void loadScriptFromFile(const std::string& file_path);

  sol::state& self();

 private:
  std::string m_script;
  sol::state m_lua_handle = nullptr;
};
}

#endif  //! __SERVER_LUA_ENGINE_HPP_
