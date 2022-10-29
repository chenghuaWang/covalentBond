#include "luaEngine.hpp"

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

std::vector<std::any> luaJitThread::execMain() {
  m_lua_handle.stack_clear();
  m_lua_handle["rhs"] = m_lua_handle.create_table_with("num", 1);
  // m_lua_handle["CBMain"]();
  sol::table abc = m_lua_handle["rhs"];
  int p = abc["num"];
  std::cout << p;
  return std::vector<std::any>();
}
}