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
struct cbOpIO {
  std::vector<cbVirtualTable> I;
  cbVirtualTable O;
};

/**
 * @brief basic operator.
 *
 */
struct baseOp {
  ~baseOp();
  baseOp();

  virtual void overload(sol::function) = 0;
  virtual void execMain();

  bool isOverload = false;
  cbOpIO io;
  opType type = opType::rowWise;
};

// Basic operation.

/**
 * @brief
 *
 */
class cbOpRowWise : public baseOp {};

/**
 * @brief
 *
 */
class cbOpTableWise : public baseOp {};

/**
 * @brief
 *
 */
class cbOpNotTable : public baseOp {};

// Details operation.

/**
 * @brief
 *
 */
class cbOpCombine : public cbOpRowWise {};

/**
 * @brief
 *
 */
class cbOpMultiMap : public cbOpRowWise {};

/**
 * @brief
 *
 */
class cbOpSort : public cbOpTableWise {};

/**
 * @brief
 *
 */
class cbOpAverage : public cbOpTableWise {};

#endif  //! __SERVER_CB_OPERATOR_HPP_