/**
 * @file cbOperator.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The operator of compute graph
 * @version 0.1
 * @date 2022-11-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "cbOperator.hpp"

baseOp::~baseOp() {}

baseOp::baseOp(const opType& ot) : type(ot) {}

cbOpRowWise::~cbOpRowWise() {}

cbOpRowWise::cbOpRowWise() : baseOp(opType::rowWise) {}

cbOpTableWise::~cbOpTableWise() {}

cbOpTableWise::cbOpTableWise() : baseOp(opType::tableWise) {}

cbOpNotTable::~cbOpNotTable() {}

cbOpNotTable::cbOpNotTable() : baseOp(opType::notTableMatrix) {}

cbOpCombine::~cbOpCombine() {}

cbOpCombine::cbOpCombine(const std::vector<std::string>& key, const std::string name)
    : m_tableName(name), m_key(key) {}

void cbOpCombine::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpCombine::execMain() { __innerFunc(this); }

void cbOpCombine::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    luaOverrideFunc((baseOp*)this, m_key, m_tableName);
  } else {
    // TODO
  }
}

cbOpMultiMap::~cbOpMultiMap() {}

void cbOpMultiMap::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpMultiMap::execMain() {
  // TODO
}

void cbOpMultiMap::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    // TODO
  } else {
  }
}

cbOpFilter::~cbOpFilter() {}

void cbOpFilter::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpFilter::execMain() {
  // TODO
}

void cbOpFilter::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    // TODO
  } else {
  }
}

cbOpSort::~cbOpSort() {}

void cbOpSort::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpSort::execMain() {
  // TODO
}

void cbOpSort::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    // TODO
  } else {
  }
}

cbOpAverage::~cbOpAverage() {}

void cbOpAverage::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpAverage::execMain() {
  // TODO
}

void cbOpAverage::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    // TODO
  } else {
  }
}

cbOpVar::~cbOpVar() {}

void cbOpVar::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpVar::execMain() {
  // TODO
}

void cbOpVar::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    // TODO
  } else {
  }
}

cbOpSum::~cbOpSum() {}

void cbOpSum::overload(const sol::function& func) {
  luaOverrideFunc = func;
  isOverload = true;
}

void cbOpSum::execMain() {
  // TODO
}

void cbOpSum::__innerFunc(baseOp* thisOp) {
  if (isOverload) {
    // TODO
  } else {
  }
}
