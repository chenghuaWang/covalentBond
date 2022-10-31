/**
 * @file luaEngine.cpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The impl of lua engine and meta class.
 * @version 0.1
 * @date 2022-10-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "luaEngine.hpp"

std::string luaTypeReminder(int32_t nums, std::vector<std::vector<__baseObj>>& rhs,
                            const std::vector<__baseObj>& outs) {
  int32_t cnt = 0;
  std::stringstream ss;
  // re-write the data types.
  ss << "--[[\n";
  // loop the vector, get all datatypes.
  ss << __LUA_IN_SCRIPT_AUTO_GEN_(nums);
  ss << "Input:\n\n";
  for (auto& ele : rhs) {
    ss << "Input-nums=" << cnt++ << "\n";
    for (auto& item : ele) {
      ss << "[key:  ] " << item.m_name << ", ";
      ss << "[Type: ] ";
      switch (item.m_type) {
        case __baseType::Int: ss << "Int, "; break;
        case __baseType::Float: ss << "Float, "; break;
        case __baseType::String: ss << "String, "; break;
      }
      ss << "\n";
    }
  }
  ss << "\nOutput:\n\n";
  for (auto& item : outs) {
    ss << "[key:  ] " << item.m_name << ", ";
    ss << "[Type: ] ";
    switch (item.m_type) {
      case __baseType::Int: ss << "Int, "; break;
      case __baseType::Float: ss << "Float, "; break;
      case __baseType::String: ss << "String, "; break;
    }
    ss << "\n";
  }
  ss << "]]\n\n";
  // re-write the main functions.
  ss << "function CBMain()\n\n";
  ss << "end\n";
  return ss.str();
}

extern "C" {
void luaJitThread::loadScript2Mem(const std::string& rhs) {
  m_script = rhs;
  m_lua_handle.open_libraries(sol::lib::base);
  m_lua_handle.script(m_script);
}

void luaJitThread::loadScriptFromFile(const std::string& file_path) {
  std::ifstream fi(file_path);
  if (fi.good()) {
    std::string str((std::istreambuf_iterator<char>(fi)), std::istreambuf_iterator<char>());
    loadScript2Mem(str);
    fi.close();
  } else {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
               "[ Error ] Failed when read file={} to lua execute Engine!\n", file_path);
  }
}

sol::state& luaJitThread::self() { return m_lua_handle; }

void luaJitThread::execMain(int32_t nums, std::vector<std::vector<__baseObj>>& rhs,
                            std::vector<__baseObj>& outs) {
  // Clear current stack. Erase previous data.
  m_lua_handle.stack_clear();
  if (nums != rhs.size()) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "Nums{} != rhs.size({})", nums,
               rhs.size());
    return;
  }
  // Pass all data to lua JIT stack.
  int32_t cnt = 0;
  std::string __keyBuffer;
  for (auto& ele : rhs) {
    ++cnt;
    __keyBuffer = "atom_" + std::to_string(cnt);
    m_lua_handle[__keyBuffer] = m_lua_handle.create_table();
    for (auto& item : ele) { __TYPE_REINTERPRET_(m_lua_handle[__keyBuffer][item.m_name], item); }
  }
  for (auto& item : outs) {
    m_lua_handle["ans"] = m_lua_handle.create_table();
    __TYPE_REINTERPRET_(m_lua_handle["ans"][item.m_name], item);
  }
  // Get data from lua JIT Stack.
  for (auto& item : outs) { __TYPE_REINTERPRET_GIVE(item, m_lua_handle["ans"][item.m_name]); }
}
}