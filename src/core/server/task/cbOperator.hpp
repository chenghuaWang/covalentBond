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
  virtual ~baseOp();
  baseOp(const opType& ot);

  virtual void overload(const sol::function& func) = 0;
  virtual void execMain() = 0;

  cbOpIO io;
  opType type = opType::rowWise;

  bool isOverload = false;
  sol::function luaOverrideFunc = sol::nil;
};

// Basic operation.

/**
 * @brief
 *
 */
class cbOpRowWise : public baseOp {
 public:
  ~cbOpRowWise();
  cbOpRowWise();
  virtual void overload(const sol::function& func) = 0;
  virtual void execMain() = 0;
  virtual void __innerFunc(baseOp* thisOp = nullptr) = 0;  ///! Stream.
};

/**
 * @brief
 *
 */
class cbOpTableWise : public baseOp {
 public:
  ~cbOpTableWise();
  cbOpTableWise();
  virtual void overload(const sol::function& func) = 0;
  virtual void execMain() = 0;
  virtual void __innerFunc(baseOp* thisOp = nullptr) = 0;  ///! Batch.
};

/**
 * @brief
 *
 */
class cbOpNotTable : public baseOp {
 public:
  ~cbOpNotTable();
  cbOpNotTable();
  virtual void overload(const sol::function& func) = 0;
  virtual void execMain() = 0;
  virtual void __innerFunc(baseOp* thisOp = nullptr) = 0;  ///! Batch
};

// Details operation.

/**
 * @brief
 *
 */
class cbOpCombine final : public cbOpRowWise {
 public:
  ~cbOpCombine() override;
  cbOpCombine(const std::vector<std::string>& key, const std::string name);
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;

 private:
  const std::string m_tableName;
  const std::vector<std::string> m_key;
};

/**
 * @brief
 *
 */
class cbOpMultiMap : public cbOpRowWise {
 public:
  ~cbOpMultiMap() override;
  cbOpMultiMap() = default;
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;
};

/**
 * @brief
 *
 */
class cbOpFilter : public cbOpTableWise {
 public:
  ~cbOpFilter() override;
  cbOpFilter() = default;
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;
};

/**
 * @brief
 *
 */
class cbOpSort : public cbOpTableWise {
 public:
  ~cbOpSort() override;
  cbOpSort() = default;
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;
};

/**
 * @brief
 *
 */
class cbOpAverage : public cbOpTableWise {
 public:
  ~cbOpAverage() override;
  cbOpAverage() = default;
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;
};

/**
 * @brief
 *
 */
class cbOpVar : public cbOpTableWise {
 public:
  ~cbOpVar() override;
  cbOpVar() = default;
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;
};

/**
 * @brief
 *
 */
class cbOpSum : public cbOpTableWise {
 public:
  ~cbOpSum() override;
  cbOpSum() = default;
  void overload(const sol::function& func) override final;
  void execMain() override final;
  void __innerFunc(baseOp* thisOp = nullptr) override final;
};

#endif  //! __SERVER_CB_OPERATOR_HPP_