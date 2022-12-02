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

void bindAllFuncByDefault(sol::state* handle) {
  (*handle).script_file("/home/wang/covalentBond/scripts/libCB.lua");

  auto covalentBound = (*handle)["Cb"].get_or_create<sol::table>();
  auto covalentBoundF = covalentBound["F"].get_or_create<sol::table>();
  // Bind Make Shape Function
  covalentBoundF.set_function("makeShapeFull", makeShapeFull);
  covalentBoundF.set_function("makeShapeCol", makeShapeCol);
  covalentBoundF.set_function("makeShapeRow", makeShapeRow);

  // Bind MySQLCell to lua.
  covalentBound.new_usertype<cbMySQLCell>(

      "KVCell",

      sol::meta_function::construct,
      sol::factories(
          [](const float& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const double& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const std::string& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const int& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const unsigned long long& value) { return MAKE_SHARED(cbMySQLCell)(value); }),

      sol::call_constructor,
      sol::factories(
          [](const float& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const double& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const std::string& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const int& value) { return MAKE_SHARED(cbMySQLCell)(value); },
          [](const unsigned long long& value) { return MAKE_SHARED(cbMySQLCell)(value); }),

      "asInt", &cbMySQLCell::asInt,

      "asFloat", &cbMySQLCell::asFloat,

      "asDouble", &cbMySQLCell::asDouble,

      "asULL", &cbMySQLCell::asULL,

      "asString", &cbMySQLCell::asString,

      "asTime", &cbMySQLCell::asTime,

      "asDataTime", &cbMySQLCell::asDatetime,

      "setInt", &cbMySQLCell::setInt,

      "setFloat", &cbMySQLCell::setFloat,

      "setULL", &cbMySQLCell::setULL,

      "setString", &cbMySQLCell::setString,

      "setTime", &cbMySQLCell::setTime,

      "setDataTime", &cbMySQLCell::setDatetime,

      "isInt", &cbMySQLCell::isInt,

      "isFloat", &cbMySQLCell::isFloat,

      "isULL", &cbMySQLCell::isULL,

      "isString", &cbMySQLCell::isString,

      "isTime", &cbMySQLCell::isTime,

      "isDataTime", &cbMySQLCell::isDatetime,

      "isDouble", &cbMySQLCell::isDouble

  );
}

luaJitThread::~luaJitThread() { delete m_luaHandle; }

luaJitThread::luaJitThread() : m_luaHandle(new sol::state()) {
  m_luaHandle->open_libraries(sol::lib::base);
  bindAllFuncByDefault(m_luaHandle);
}

void luaJitThread::execMain(const std::string& script) { m_luaHandle->script(script); }

sol::state& luaJitThread::operator()() { return *m_luaHandle; }
