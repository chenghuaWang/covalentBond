/**
 * @file cbObject.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief A python meta object like, type-free objects for structure loading
 * different type's data.
 * @version 0.1
 * @date 2022-10-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __UTILS_CB_OBJECT_HPP_
#define __UTILS_CB_OBJECT_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../pch.hpp"

/**
 * @brief
 *
 */
enum class __metaType_ : uint8_t {
  Type = 0,
  Int = 1,
  Float = 2,
  String = 3,
  List = 4,
  Expression = 5,
};

// Below implement all metaObject based objects.

class cbInt;
class cbFloat;
class cbString;
class cbList;

/**
 * @brief
 *
 * @tparam ObjT
 * @tparam T
 */
template<typename ObjT, __metaType_ T>
struct __metaObj_ {
  CB_INLINE_NORMAL ObjT* selfPtr() { return static_cast<ObjT*>(this); }
  CB_INLINE_NORMAL const ObjT& selfRef() const { return static_cast<const ObjT>(*this); }

  __metaType_ m_type = T;
};

/**
 * @brief
 *
 */
class cbInt : public __metaObj_<cbInt, __metaType_::Int> {
 public:
  cbInt(int32_t rhs) : __data(rhs) {}
  CB_INLINE_NORMAL int32_t getData() { return __data; }

 private:
  int32_t __data;
};

// Below to implement all expression based.

/**
 * @brief
 *
 * @tparam ObjT_1
 * @tparam ObjT_2
 * @tparam T_1
 * @tparam T_2
 */
template<typename ObjT_1, typename ObjT_2, __metaType_ T_1, __metaType_ T_2>
class binaryExpression
    : __metaObj_<binaryExpression<ObjT_1, ObjT_2, T_1, T_2>, __metaType_::Expression> {
 public:
  explicit binaryExpression(const ObjT_1& exp_1, const ObjT_2& exp_2)
      : m_exp_1(exp_1), m_exp_2(exp_2) {}

 private:
  const ObjT_1& m_exp_1;
  const ObjT_2& m_exp_2;
};

#endif  //! __UTILS_CB_OBJECT_HPP_