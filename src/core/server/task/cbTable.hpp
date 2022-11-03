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
#include <workflow/MySQLResult.h>

///!
typedef std::variant<int32_t, float, std::string, bool> __metaObj;

///!
#define Seq(x, y) std::pair<int32_t, int32_t>(x, y)

/**
 * @brief
 *
 */
enum class cbTableSlice : uint8_t {
  all = 0,
  seq = 1,
  no = 2,
};

/**
 * @brief
 *
 */
enum class typeType : uint32_t {
  Int = 0,
  Float = 1,
  String = 2,
  Bool = 3,
};

/**
 * @brief
 *
 */
struct cbCell {
  cbCell() = default;
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

/**
 * @brief
 *
 */
struct cbTableHead {
 public:
  cbTableHead();
  cbTableHead(const int32_t& nums) : m_nums(nums), m_data(nums), m_types(nums) {}
  cbTableHead(const int32_t& nums, const std::initializer_list<std::string>& data,
              const std::initializer_list<typeType>& types)
      : m_nums(nums), m_data(data) {}

  std::vector<std::string>::iterator begin() { return m_data.begin(); }
  std::vector<std::string>::iterator end() { return m_data.end(); }
  const std::vector<std::string>::const_iterator cbegin() { return m_data.cbegin(); }
  const std::vector<std::string>::const_iterator cend() { return m_data.cend(); }

  std::pair<std::string&, typeType&> operator[](int32_t idx) { return {m_data[idx], m_types[idx]}; }

  void set(const std::string& n, int32_t idx) {
    if (idx >= 0 && idx < m_nums) m_data[idx] = n;
  }

  void set(const typeType& t, int32_t idx) {
    if (idx >= 0 && idx < m_nums) m_types[idx] = t;
  }

  void addNameAndType(const std::string& name, const typeType& t) {
    m_data.push_back(name);
    m_types.push_back(t);
  }

 private:
  int32_t m_nums = 0;
  std::vector<std::string> m_data;
  std::vector<typeType> m_types;
};

/**
 * @brief
 *
 * @tparam Dims
 */
template<int32_t Dims>
struct cbShape {
  cbShape() = default;
  cbShape(const cbShape<Dims>& rhs) {
#pragma unroll
    for (int32_t i = 0; i < Dims; ++i) { this->m_dims[i] = rhs.m_dims[i]; }
    this->m_stride = rhs.m_stride;
  }
  cbShape(const std::initializer_list<int32_t>& rhs) : m_dims(rhs) {}

  inline int32_t& operator[](int32_t idx) { return m_dims[idx]; }

  int32_t numElements() {
    int32_t ans = 1;
#pragma unroll
    for (int32_t i = 0; i < Dims; ++i) { ans *= m_dims[i]; }
    return ans;
  }

  int32_t m_stride;
  int32_t m_dims[Dims];
};

/**
 * @brief
 *
 * @param a
 * @return cbShape<2>
 */
inline cbShape<2> makeShapeRow(int32_t a) {
  cbShape<2> ans;
  ans[0] = 1;
  ans[1] = a;
  return ans;
}

/**
 * @brief
 *
 * @param a
 * @return cbShape<2>
 */
inline cbShape<2> makeShapeCol(int32_t a) {
  cbShape<2> ans;
  ans[0] = a;
  ans[1] = 1;
  return ans;
}

/**
 * @brief
 *
 * @param a
 * @param b
 * @return cbShape<2>
 */
inline cbShape<2> makeShapeFull(int32_t a, int32_t b) {
  cbShape<2> ans;
  ans[0] = a;
  ans[1] = b;
  return ans;
}

/**
 * @brief row major table.
 *
 * @tparam Dims
 */
template<int32_t Dims>
struct __metaMatrix {
  __metaMatrix() = default;
  __metaMatrix(const cbShape<Dims>& shape) : m_shape(shape) {}
  __metaMatrix(const cbShape<Dims>& shape, cbCell* dataptr) : m_shape(shape), __dataptr(dataptr) {}
  __metaMatrix(const __metaMatrix& rhs) {
    this->m_shape = rhs.m_shape;
    this->__dataptr = rhs.__dataptr;
  }

  void free() {
    if (m_trueDataContainer) { delete __dataptr; }
  }

  cbShape<Dims> m_shape;
  cbCell* __dataptr = nullptr;
  bool m_trueDataContainer = false;
};

/**
 * @brief
 *
 */
struct cbTableRow : public __metaMatrix<2> {
 public:
  cbTableRow() = delete;
  cbTableRow(const cbShape<2>& shape) : __metaMatrix<2>(shape) {}
  cbTableRow(const cbShape<2>& shape, cbCell* dataptr) : __metaMatrix<2>(shape, dataptr) {}
  cbTableRow(const cbTableRow& rhs) {
    this->__dataptr = rhs.__dataptr;
    this->m_shape = rhs.m_shape;
  }

  void setThisAsTrueDataContainer() { m_trueDataContainer = true; }

  cbCell* at(int32_t i) { return __dataptr + i; }

  cbTableHead head;
};

/**
 * @brief
 *
 */
struct cbTableCol : public __metaMatrix<2> {
 public:
  cbTableCol() = delete;
  cbTableCol(const cbShape<2>& shape) : __metaMatrix<2>(shape), m_stride(1) {}
  cbTableCol(const cbShape<2>& shape, int32_t stride) : __metaMatrix<2>(shape), m_stride(stride) {}
  cbTableCol(const cbShape<2>& shape, int32_t stride, cbCell* dataptr)
      : __metaMatrix<2>(shape, dataptr), m_stride(stride) {}
  cbTableCol(const cbTableCol& rhs) {
    this->__dataptr = rhs.__dataptr;
    this->m_shape = rhs.m_shape;
    this->m_stride = rhs.m_stride;
  }

  cbCell* at(int32_t i) { return __dataptr + i * m_stride; }

  void setThisAsTrueDataContainer() { m_trueDataContainer = true; }

  void setStride(int32_t rhs) { m_stride = rhs; }

  cbTableHead head;

 private:
  int32_t m_stride = 0;
};

/**
 * @brief
 *
 */
class cbTable : public __metaMatrix<2> {
 public:
  cbTable() = delete;
  cbTable(const cbTable& rhs) {
    this->m_shape = rhs.m_shape;
    this->__dataptr = rhs.__dataptr;
    this->m_stride = this->m_shape[1];
  }
  cbTable(cbShape<2>& shape) : __metaMatrix<2>(shape), m_stride(shape[1]) {}
  cbTable(cbShape<2>& shape, cbCell* dataptr)
      : __metaMatrix<2>(shape, dataptr), m_stride(shape[1]) {}

  void setThisAsTrueDataContainer() { m_trueDataContainer = true; }

  inline cbCell* at(int32_t i, int32_t j) { return __dataptr + (i * m_stride + j); }

  inline cbCell* operator()(int32_t i, int32_t j) { return at(i, j); }

  inline cbTable operator()(const std::pair<int32_t, int32_t>& row_i, const cbTableSlice& cbSl) {
    int32_t rows = row_i.second - row_i.first;
    int32_t cols = 0;
    switch (cbSl) {
      case cbTableSlice::all: cols = m_shape[1]; break;
      case cbTableSlice::no: break;
      case cbTableSlice::seq: break;
    }
    cbShape<2> res = makeShapeFull(rows, cols);

    cbTable ans(res, __dataptr);
    ans.head = this->head;

    return ans;
  }

  inline cbTableRow getRow(int32_t i) {
    cbTableRow ans(makeShapeRow(m_shape[1]), __dataptr + i * m_stride);
    ans.head = this->head;
    return ans;
  }

  inline cbTableCol getCol(int32_t i) {
    cbTableCol ans(makeShapeCol(m_shape[0]), m_stride, __dataptr + i);
    auto tmp_heads_ele = this->head[i];
    ans.head = cbTableHead(1);
    ans.head.set(tmp_heads_ele.first, i);
    ans.head.set(tmp_heads_ele.second, i);
    return ans;
  }

  cbTableHead head;

 private:
  int32_t m_stride = 0;
};

/**
 * @brief
 *
 * @param rhs
 */
inline void mallocMetaMatrix(cbTable& rhs) {
  rhs.__dataptr = new cbCell[rhs.m_shape.numElements()];
  rhs.setThisAsTrueDataContainer();
}

/**
 * @brief
 *
 * @param rhs
 */
inline void mallocMetaMatrix(cbTableCol& rhs) {
  rhs.__dataptr = new cbCell[rhs.m_shape.numElements()];
  rhs.setStride(1);
  rhs.setThisAsTrueDataContainer();
}

/**
 * @brief cbVirtualSharedTable is a container of shared memory.
 *
 */
class cbVirtualSharedTable {
 public:
  ~cbVirtualSharedTable();
  cbVirtualSharedTable() = default;
  /**
   * @brief Construct a new cb Virtual Shared Table object
   * Only loop one result set.
   * @param cursor
   */
  cbVirtualSharedTable(protocol::MySQLResultCursor* cursor);
  const cbShape<2> getShape() const { return m_shape; }
  protocol::MySQLCell* at(int32_t i, int32_t j);
  protocol::MySQLField** getInfo() { return m_info; }
  int32_t getFiledCount() { return m_fieldCount; }

 private:
  int32_t m_fieldCount = 0;
  cbShape<2> m_shape;
  protocol::MySQLField** m_info = nullptr;
  std::vector<std::vector<protocol::MySQLCell>> m_data;
};

/**
 * @brief cbVirtualTable works as a reference from shared memory. It
 * only use a shape and SqlCell to define different tables.
 *
 */
class cbVirtualTable {
 public:
  cbVirtualTable() = default;
  cbVirtualTable(const cbVirtualTable& rhs) {
    this->m_shape = rhs.m_shape;
    this->m_data = rhs.m_data;
    this->m_info = rhs.m_info;
  }
  cbVirtualTable(cbShape<2>& shape)
      : m_info(nullptr),
        m_shape(shape),
        m_data(shape[0], std::vector<protocol::MySQLCell*>(shape[1])) {
    m_data.shrink_to_fit();
  }

  cbVirtualTable operator=(const cbVirtualTable& rhsOp) {
    cbVirtualTable ans;
    ans.m_data = rhsOp.m_data;
    ans.m_info = rhsOp.m_info;
    ans.m_shape = rhsOp.m_shape;
    return ans;
  }

  void resetShape(const cbShape<2>& shape);
  protocol::MySQLField** getInfo();
  std::vector<std::vector<protocol::MySQLCell*>>& getData();
  cbShape<2> getShape() { return m_shape; }

  /**
   * @brief row major
   *
   * @param i row
   * @param j col
   * @return protocol::MySQLCell*
   */
  protocol::MySQLCell* at(int32_t i, int32_t j);

  std::string colNameAt(int32_t i);
  std::string colTypeAt(int32_t i);

 private:
  protocol::MySQLField** m_info = nullptr;
  cbShape<2> m_shape;
  std::vector<std::vector<protocol::MySQLCell*>> m_data;
};

void mapShared2Virtual(cbVirtualSharedTable* sharedT, cbVirtualTable* virtualT);

#endif  //! __SERVER_CB_TABLE_HPP_