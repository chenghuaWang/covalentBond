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

class baseOp {};

#endif  //! __SERVER_CB_OPERATOR_HPP_