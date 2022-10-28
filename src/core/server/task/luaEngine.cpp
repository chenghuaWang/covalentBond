#include "luaEngine.hpp"

extern "C" {
void luaJitThread::loadScript2Mem(const std::string& rhs) { m_script = rhs; }

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
}