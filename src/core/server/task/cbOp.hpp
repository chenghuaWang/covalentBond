/**
 * @file cbOp.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The luaJitThread will be packed into a operator class.
 * In this operator, value types(both inputs and outputs) and lua
 * Jit Thread exists.
 * @version 0.1
 * @date 2022-10-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __SERVER_CB_OP_HPP_
#define __SERVER_CB_OP_HPP_

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

class cbOperator;

/**
 * @brief The inner function which actually parse the json file to type object.
 *
 * @param ele The operator itself, for friend class to pass in.
 * @param keyType The Json String.
 */
void __parseKeyTypeFromJson(cbOperator* ele, const std::string keyType);

/**
 * @brief The operator class need record 3 basic things.
 * 1. How many values need to be performed by this operation.
 * 2. What those values looks like. (Type, Name, etc)
 * 3. How to calculate those values. (Sql sentence, lua script)
 *
 */
class cbOperator {
 public:
  cbOperator() = delete;
  cbOperator operator=(const cbOperator) = delete;

  friend void __parseKeyTypeFromJson(cbOperator* ele, const std::string keyType);

  /**
   * @brief Construct a new cb Operator object
   * @note You should remind the format of the json input should follow the example below
   * {
   *    "inputsValueNums": 3,
   *    "sentence": {
   *      "atom_1": "SELECT {1} FROM {2}",
   *      "atom_2": "SELECT {1} FROM {2}",
   *      "atom_3": "SELECT {1} FROM {2}",
   *    }
   *    "inputs": {
   *        "atom_1": {
   *            "foo": "Int",
   *            "li": "Float",
   *        },
   *        "atom_2": {
   *            "foo": "Int",
   *            "li": "Float",
   *        },
   *        "atom_3": {
   *            "foo": "Int",
   *            "li": "Float",
   *        },
   *    },
   *    "outputs": {
   *        "foo": "Int",
   *        "li": "Float",
   *    }
   * }
   *
   * @param keyType Json string.
   * @param luaOp lua script string.
   */
  explicit cbOperator(const std::string keyType, const std::string luaOp);
  void reBind(const std::string keyType, const std::string luaOp);

 private:
  std::vector<std::string> m_sentence;
  std::vector<std::unordered_map<std::string, std::string>> m_keyTypeIn;
  std::unordered_map<std::string, std::string> m_KeyTypeOut;
  SHARED_PTR(luaJitThread) m_luaJitHandle = nullptr;
};

#endif  //! __SERVER_CB_OP_HPP_