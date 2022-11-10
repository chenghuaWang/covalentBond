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
#ifndef __SERVER_CB_OPERATOR_HPP_
#define __SERVER_CB_OPERATOR_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../../pch.hpp"

#include "luaEngine.hpp"

enum class opType : uint32_t {
  rowWise = 0,
  tableWise = 1,
  notTableMatrix = 2,
};

/**
 * @brief The input and output of one Operator.
 *
 */
struct cbOpIO {};

/**
 * @brief basic operator.
 *
 */
struct baseOp {
  ~baseOp(){};
  baseOp(){};

  virtual void overload(sol::function) = 0;
  virtual void execMain();

  bool isOverload = false;
  cbOpIO io;
  opType type = opType::rowWise;
};

class cbOpRowWise : public baseOp {};

class cbOpTableWise : public baseOp {};

class cbOpNotTable : public baseOp {};

#endif  //! __SERVER_CB_OPERATOR_HPP_