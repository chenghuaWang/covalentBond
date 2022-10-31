#ifndef __SERVER_CB_TABLE_HPP_
#define __SERVER_CB_TABLE_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)
#include <variant>

#include "../../pch.hpp"
#include "sol/sol.hpp"

typedef std::variant<int32_t, float, std::string, bool> __metaObj;

#define Seq(x, y) std::pair<int32_t, int32_t>(x, y)

enum class cbTableSlice : uint8_t {
  all = 0,
  seq = 1,
  no = 2,
};

struct cbCell {
  cbCell() = delete;
  cbCell(const cbCell& rhs) = delete;
  cbCell operator=(const cbCell& rhs) = delete;
  cbCell(int32_t rhs) : data(rhs) {}
  cbCell(float rhs) : data(rhs) {}
  cbCell(bool rhs) : data(rhs) {}
  cbCell(const std::string& rhs) : data(rhs) {}
  // get data
  int asInt();
  float asFloat();
  bool asBool();
  std::string& asString();
  // set data
  void setAsLuaInt(sol::object rhs);
  void setAsLuaFloat(sol::object rhs);
  void setAsLuaBool(sol::object rhs);
  void setAsLuaString(sol::object rhs);
  // set data
  void setAsCppInt(int32_t rhs);
  void setAsCppFloat(float rhs);
  void setAsCppBool(bool rhs);
  void setAsCppString(std::string rhs);
  __metaObj data;
};

struct cbTableHead {
 private:
  int32_t m_nums = 0;
  std::vector<std::string> m_data;
};

template<int32_t Dims>
struct cbShape {
  cbShape() = default;
  cbShape(const cbShape& rhs) {
    this->m_dims = rhs.m_dims;
    this->m_stride = rhs.m_stride;
  }
  cbShape(const std::initializer_list<int32_t>& rhs) : m_dims(rhs) {}

  inline const int32_t& operator[](int32_t idx) { return m_dims[idx]; }

  int32_t m_stride;
  int32_t m_dims[Dims];
};

cbShape<1> makeShapeRow(int32_t a);
cbShape<2> makeShapeCol(int32_t a);
cbShape<2> makeShapeFull(int32_t a, int32_t b);

template<int32_t Dims>
struct __metaMatrix;

template<int32_t Dims>
inline void mallocMetaMatrix(__metaMatrix<Dims>* a) {}

template<int32_t Dims>
inline void freeMetaMatrix(__metaMatrix<Dims>* a) {}

template<int32_t Dims>
struct __metaMatrix {
  __metaMatrix() = default;
  __metaMatrix(const __metaMatrix& rhs) {
    this->m_shape = rhs.m_shape;
    this->__dataptr = rhs.__dataptr;
  }

  void free() { freeMetaMatrix(this); }

 private:
  cbShape<Dims> m_shape;
  cbCell* __dataptr = nullptr;
};

struct cbTableRow : protected __metaMatrix<1> {};

struct cbTableCol : protected __metaMatrix<2> {};

class cbTable : protected __metaMatrix<2> {
 public:
  bool m_trueDataContainer = false;
};

#endif  //! __SERVER_CB_TABLE_HPP_