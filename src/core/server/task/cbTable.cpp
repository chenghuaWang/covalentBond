#include "cbTable.hpp"

int cbCell::asInt() { return std::get<int>(data); }

float cbCell::asFloat() { return std::get<float>(data); }

bool cbCell::asBool() { return std::get<bool>(data); }

std::string& cbCell::asString() { return std::get<std::string>(data); }

void cbCell::setAsLuaInt(sol::object rhs) { data = rhs.as<int>(); }

void cbCell::setAsLuaFloat(sol::object rhs) { data = rhs.as<float>(); }

void cbCell::setAsLuaBool(sol::object rhs) { data = rhs.as<bool>(); }

void cbCell::setAsLuaString(sol::object rhs) { data = rhs.as<std::string>(); }

void cbCell::setAsCppInt(int32_t rhs) { data = rhs; }

void cbCell::setAsCppFloat(float rhs) { data = rhs; }

void cbCell::setAsCppBool(bool rhs) { data = rhs; }

void cbCell::setAsCppString(std::string rhs) { data = rhs; }
