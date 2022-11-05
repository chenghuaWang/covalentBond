#include "cbTable.hpp"
#include <workflow/MySQLResult.h>

// ------------------------- The code below is dropped.--------------
// --- Go to len=340 for latest code. -------------------------------
// ------------------------- The code below is dropped.--------------

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

// -------------- below is for actually use. ---------------------------------
// -------------- below is for actually use. ---------------------------------
// -------------- below is for actually use. ---------------------------------

// And I will provide a bunch of function for sol::lua to use.

cbVirtualSharedTable::~cbVirtualSharedTable() {
  for (int32_t i = 0; i < m_fieldCount; ++i) { delete m_info[i]; }
  delete[] m_info;
}

cbVirtualSharedTable::cbVirtualSharedTable(protocol::MySQLResultCursor* cursor) {
  if (cursor->get_cursor_status() == MYSQL_STATUS_GET_RESULT) {
    // create filed buffer. move the memory from cursor to virtual table.
    m_fieldCount = cursor->get_field_count();
    m_info = new protocol::MySQLField*[m_fieldCount];
    const protocol::MySQLField* const* __tmpCursorFieldPtr =
        cursor->fetch_fields();  ///! no copy api
    for (int32_t i = 0; i < m_fieldCount; ++i) {
      m_info[i] = new protocol::MySQLField(*__tmpCursorFieldPtr[i]);
    }
    // no loop. just get the first result set !!!
    // Note that the implementation of workflow's MySqlCell use void* to store
    // different data types, so I adopt RAII programming style in this class.
    std::vector<std::vector<protocol::MySQLCell>> __data;
    cursor->fetch_all(__data);  ///! with copy.
    m_shape[0] = cursor->get_rows_count();
    m_shape[1] = static_cast<int32_t>(m_data[0].size());
    for (int32_t i = 0; i < m_shape[0]; ++i) {
      for (int32_t j = 0; j < m_shape[1]; ++j) { m_data[i][j] = __data[i][j]; }
    }
  }
}

cbMySQLCell* cbVirtualSharedTable::atPtr(int32_t i, int32_t j) { return &m_data[i][j]; }

cbMySQLCell* cbVirtualSharedTable::atPtrRef(int32_t i, int32_t j) { return &m_data[i][j]; }

void cbVirtualSharedTable::resetShape(cbShape<2>& shape) {
  m_shape = shape;
  m_data.resize(m_shape[0]);
  for (auto& item : m_data) {
    item.resize(m_shape[1]);
    item.shrink_to_fit();
  }
  m_data.shrink_to_fit();
}

void cbVirtualSharedTable::resetFieldInfo(int32_t fieldCount, protocol::MySQLField** info) {
  for (int32_t i = 0; i < m_fieldCount; ++i) { delete m_info[i]; }
  delete[] m_info;
  m_info = info;
  m_fieldCount = fieldCount;
}

void cbVirtualTable::resetShape(const cbShape<2>& shape) {
  // row major
  m_shape = shape;
  m_data.resize(m_shape[0]);  // row.
  for (auto& item : m_data) {
    item.resize(m_shape[1]);
    item.shrink_to_fit();
  }  // col.
  m_data.shrink_to_fit();
}

protocol::MySQLField** cbVirtualTable::getInfo() { return m_info; }

std::vector<std::vector<cbMySQLCell*>>& cbVirtualTable::getData() { return m_data; }

cbMySQLCell* cbVirtualTable::atPtr(int32_t i, int32_t j) { return m_data[i][j]; }

cbMySQLCell*& cbVirtualTable::atPtrRef(int32_t i, int32_t j) { return m_data[i][j]; }

std::string cbVirtualTable::colNameAt(int32_t i) { return m_info[i]->get_name(); }

std::string cbVirtualTable::colTypeAt(int32_t i) {
  return datatype2str(m_info[i]->get_data_type());
}

void mapShared2Virtual(cbVirtualSharedTable* sharedT, cbVirtualTable* virtualT) {
  virtualT->resetShape(sharedT->getShape());
  int32_t row = virtualT->getShape()[0];
  int32_t col = virtualT->getShape()[1];

  // left value is temp.
  auto p = virtualT->getInfo();
  p = sharedT->getInfo();

  for (int32_t i = 0; i < row; ++i) {
    for (int32_t j = 0; j < col; ++j) { virtualT->atPtrRef(i, j) = sharedT->atPtr(i, j); }
  }
}

cbMySQLCell::cbMySQLCell(const protocol::MySQLCell& m) {
  if (m.is_date()) {
    m_type = cbMySQLType::Date;
    m_data = m.as_date();
  } else if (m.is_datetime()) {
    m_type = cbMySQLType::DataTime;
    m_data = m.as_datetime();
  } else if (m.is_time()) {
    m_type = cbMySQLType::Time;
    m_data = m.as_time();
  } else if (m.is_float()) {
    m_type = cbMySQLType::Float;
    m_data = m.as_float();
  } else if (m.is_double()) {
    m_type = cbMySQLType::Double;
    m_data = m.as_double();
  } else if (m.is_int()) {
    m_type = cbMySQLType::Int;
    m_data = m.as_int();
  } else if (m.is_string()) {
    m_type = cbMySQLType::String;
    m_data = m.as_string();
  } else if (m.is_ulonglong()) {
    m_type = cbMySQLType::ULL;
    m_data = m.as_ulonglong();
  } else {
    m_type = cbMySQLType::Null;
  }
}

bool cbMySQLCell::isNull() const {
  switch (m_type) {
    case cbMySQLType::Null: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isInt() const {
  switch (m_type) {
    case cbMySQLType::Int: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isString() const {
  switch (m_type) {
    case cbMySQLType::String:
    case cbMySQLType::DataTime:
    case cbMySQLType::Date:
    case cbMySQLType::Time: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isFloat() const {
  switch (m_type) {
    case cbMySQLType::Float: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isDouble() const {
  switch (m_type) {
    case cbMySQLType::Double: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isULL() const {
  switch (m_type) {
    case cbMySQLType::ULL: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isDate() const {
  switch (m_type) {
    case cbMySQLType::Date: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isTime() const {
  switch (m_type) {
    case cbMySQLType::Time: return true; break;
    default: return false; break;
  }
}

bool cbMySQLCell::isDatetime() const {
  switch (m_type) {
    case cbMySQLType::DataTime: return true; break;
    default: return false; break;
  }
}

int cbMySQLCell::asInt() const {
  if (isInt()) { return std::get<int>(m_data); }
  return 0;
}

std::string cbMySQLCell::asString() const {
  if (isString()) { return std::get<std::string>(m_data); }
  return "";
}

float cbMySQLCell::asFloat() const {
  if (isFloat()) { return std::get<float>(m_data); }
  return NAN;
}

double cbMySQLCell::asDouble() const {
  if (isDouble()) { return std::get<double>(m_data); }
  return NAN;
}

unsigned long long cbMySQLCell::asULL() const {
  if (isULL()) { return std::get<unsigned long long>(m_data); }
  return 0;
}

std::string cbMySQLCell::asDate() const { return asString(); }

std::string cbMySQLCell::asTime() const { return asString(); }

std::string cbMySQLCell::asDatetime() const { return asString(); }

void cbMySQLCell::setInt(int value) {
  if (isInt()) { m_data = value; }
}

void cbMySQLCell::setString(const std::string& value) {
  if (isString()) { m_data = value; }
}

void cbMySQLCell::setFloat(float value) {
  if (isFloat()) { m_data = value; }
}

void cbMySQLCell::setDouble(double value) {
  if (isDouble()) { m_data = value; }
}

void cbMySQLCell::setULL(unsigned long long value) {
  if (isULL()) { m_data = value; }
}

void cbMySQLCell::setDate(const std::string& value) {
  if (isDate()) { m_data = value; }
}

void cbMySQLCell::setTime(const std::string& value) {
  if (isTime()) { m_data = value; }
}

void cbMySQLCell::setDatetime(const std::string& value) {
  if (isDatetime()) { m_data = value; }
}
