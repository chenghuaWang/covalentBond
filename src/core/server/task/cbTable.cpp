/**
 * @file cbTable.cpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The impl of table.
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "cbTable.hpp"
#include <workflow/MySQLResult.h>

// ------------------------- The code below is dropped.--------------
// --- Go to len=46 for latest code. -------------------------------
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
    m_info = new cbMySQLField*[m_fieldCount];
    const protocol::MySQLField* const* __tmpCursorFieldPtr =
        cursor->fetch_fields();  ///! no copy api
    for (int32_t i = 0; i < m_fieldCount; ++i) {
      m_info[i] = new cbMySQLField(__tmpCursorFieldPtr[i]);
    }
    // no loop. just get the first result set !!!
    // Note that the implementation of workflow's MySqlCell use void* to store
    // different data types, so I adopt RAII programming style in this class.
    std::vector<std::vector<protocol::MySQLCell>> __data;
    cursor->fetch_all(__data);  ///! with copy.
    m_shape[0] = cursor->get_rows_count();
    m_shape[1] = static_cast<int32_t>(__data[0].size());
    resetShape(m_shape);
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

void cbVirtualSharedTable::resetFieldInfo(int32_t fieldCount, cbMySQLField** info) {
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

void cbVirtualTable::resetShapeH(const cbShape<2>& shape) {
  resetShape(shape);
  m_isInfoExtent = true;
  m_info = new cbMySQLField*[shape[1]];
}

cbMySQLField** cbVirtualTable::getInfo() { return m_info; }

void cbVirtualTable::setInfo(cbMySQLField** v) { m_info = v; }

void cbVirtualTable::setInfoAt(int32_t i, cbMySQLField* v) {
  m_isInfoExtent = true;
  m_info[i] = v;
}

cbMySQLField* cbVirtualTable::getInfoAt(int32_t i) { return m_info[i]; }

std::vector<std::vector<cbMySQLCell*>>& cbVirtualTable::getData() { return m_data; }

cbMySQLCell* cbVirtualTable::atPtr(int32_t i, int32_t j) { return m_data[i][j]; }

cbMySQLCell*& cbVirtualTable::atPtrRef(int32_t i, int32_t j) { return m_data[i][j]; }

void cbVirtualTable::setPtrAt(int32_t i, int32_t j, cbMySQLCell* v) { atPtrRef(i, j) = v; }

cbVirtualTable cbVirtualTable::getRow(int32_t i) {
  if (i >= 0 && i < m_shape[0]) [[likely]] {
    int32_t col = m_shape[1];
    cbVirtualTable tmp(makeShapeFull(1, col));
    for (int32_t c = 0; c < col; ++c) { tmp.atPtrRef(0, c) = this->atPtrRef(i, c); }
    tmp.setInfo(this->m_info);
    return tmp;
  }
  return cbVirtualTable(makeShapeFull(0, 0));
}

cbVirtualTable cbVirtualTable::getCol(int32_t i) {
  if (i >= 0 && i < m_shape[1]) [[likely]] {
    int32_t row = m_shape[0];
    cbVirtualTable tmp(makeShapeFull(row, 1));
    for (int32_t r = 0; r < row; ++r) { tmp.atPtrRef(r, 0) = this->atPtrRef(r, i); }
    tmp.setInfo(this->getInfo() + i);
    return tmp;
  }
  return cbVirtualTable(makeShapeFull(0, 0));
}

std::map<int32_t, int32_t> cbVirtualTable::keyBy(const std::string& colName) const {
  std::map<int32_t, int32_t> tmp;
  int32_t row = m_shape[0];
  int32_t col = m_shape[1];
  int atCol = 0;
  // hash funcs
  std::hash<std::string> hash_string;
  [[maybe_unused]] std::hash<float> hash_float;
  [[maybe_unused]] std::hash<double> hash_double;
  [[maybe_unused]] std::hash<int> hash_int;
  [[maybe_unused]] std::hash<unsigned long long> hash_ull;

  // loops
  for (atCol = 0; atCol < col; ++atCol) {
    if (m_info[atCol]->getName() == colName) break;
  }
  if (atCol == col) [[unlikely]] { return tmp; }
  for (int32_t r = 0; r < row; ++r) {
    switch (m_data[r][atCol]->getType()) {
      case cbMySQLType::String:
      case cbMySQLType::DataTime:
      case cbMySQLType::Date:
      case cbMySQLType::Time: tmp[hash_string(m_data[r][atCol]->asString())] = r; break;
      case cbMySQLType::Float: tmp[hash_float(m_data[r][atCol]->asFloat())] = r; break;
      case cbMySQLType::Double: tmp[hash_double(m_data[r][atCol]->asFloat())] = r; break;
      case cbMySQLType::Int: tmp[hash_int(m_data[r][atCol]->asInt())] = r; break;
      case cbMySQLType::ULL: tmp[hash_ull(m_data[r][atCol]->asULL())] = r; break;
      case cbMySQLType::Null: return tmp;
    }
  }
  return tmp;
}

void cbVirtualTable::pushRow(const std::vector<cbMySQLCell*>& row) {
  if (row.size() != m_shape[1]) [[unlikely]] { return; }
  m_data.push_back(row);
  m_shape[0]++;
}

std::string cbVirtualTable::colNameAt(int32_t i) { return m_info[i]->getName(); }

std::string cbVirtualTable::colTypeAt(int32_t i) { return datatype2str(m_info[i]->getDataType()); }

void cbVirtualTable::str() {
  fmt::print(fg(fmt::color::hot_pink) | fmt::emphasis::italic, "<Table, row={}, col={}>\n",
             m_shape[0], m_shape[1]);
}

cbOutputTableStruct::cbOutputTableStruct(const cbShape<2>& shape, cbMySQLField** info)
    : m_shape(shape) {
  int32_t col = m_shape[1];
  for (int32_t i = 0; i < col; ++i) { m_info.push_back(cbMySQLField(info[i])); }
}

void cbOutputTableStruct::clear() {
  m_info.clear();
  m_info.shrink_to_fit();
  m_shape[0] = 0;
  m_shape[1] = 0;
}

void cbOutputTableStruct::update(const cbShape<2>& shape, cbMySQLField** info) {
  clear();
  m_shape = shape;
  int32_t col = m_shape[1];
  m_info.clear();
  m_info.shrink_to_fit();
  for (int32_t i = 0; i < col; ++i) { m_info.push_back(cbMySQLField(info[i])); }
}

std::string cbOutputTableStruct::genKey4Redis(int32_t row, int32_t col) {
  std::stringstream ss;
  ss << m_info[col].getTable() << ":" << row << ":" << m_info[col].getName();
  return ss.str();
}

void mapShared2Virtual(cbVirtualSharedTable* sharedT, cbVirtualTable* virtualT) {
  virtualT->resetShape(sharedT->getShape());
  int32_t row = virtualT->getShape()[0];
  int32_t col = virtualT->getShape()[1];

  // left value is temp.
  virtualT->setInfo(sharedT->getInfo());
  virtualT->resetShape(makeShapeFull(row, col));
  for (int32_t i = 0; i < row; ++i) {
    for (int32_t j = 0; j < col; ++j) { virtualT->atPtrRef(i, j) = sharedT->atPtr(i, j); }
  }
}

cbMySQLField::cbMySQLField() {}

cbMySQLField::cbMySQLField(const protocol::MySQLField* wfPtr) {
  m_name = wfPtr->get_name();
  m_orgName = wfPtr->get_org_name();
  m_table = wfPtr->get_table();
  m_orgTable = wfPtr->get_org_table();
  m_db = wfPtr->get_db();
  m_catalog = wfPtr->get_catalog();
  m_def = wfPtr->get_def();
  m_length = wfPtr->get_length();
  m_flags = wfPtr->get_flags();
  m_decimals = wfPtr->get_decimals();
  m_charsetnr = wfPtr->get_charsetnr();
  m_data_type = wfPtr->get_data_type();
}

cbMySQLField::cbMySQLField(const cbMySQLField* ptr) {
  m_name = ptr->getName();
  m_orgName = ptr->getOrgName();
  m_table = ptr->getTable();
  m_orgTable = ptr->getOrgTable();
  m_db = ptr->getDB();
  m_catalog = ptr->getCatalog();
  m_def = ptr->getDef();
  m_length = ptr->getLength();
  m_flags = ptr->getFlags();
  m_decimals = ptr->getDecimals();
  m_charsetnr = ptr->getCharsetnr();
  m_data_type = ptr->getDataType();
}

// get data.
std::string cbMySQLField::getName() const { return m_name; }

std::string cbMySQLField::getOrgName() const { return m_orgName; }
std::string cbMySQLField::getTable() const { return m_table; }
std::string cbMySQLField::getOrgTable() const { return m_orgTable; }
std::string cbMySQLField::getDB() const { return m_db; }
std::string cbMySQLField::getCatalog() const { return m_catalog; }
std::string cbMySQLField::getDef() const { return m_def; }
int cbMySQLField::getCharsetnr() const { return m_charsetnr; }
int cbMySQLField::getLength() const { return m_length; }
int cbMySQLField::getFlags() const { return m_flags; }
int cbMySQLField::getDecimals() const { return m_decimals; }
int cbMySQLField::getDataType() const { return m_data_type; }

// set data.
void cbMySQLField::setName(const std::string& value) { m_name = value; }
void cbMySQLField::setOrgName(const std::string& value) { m_orgName = value; }
void cbMySQLField::setTable(const std::string& value) { m_table = value; }
void cbMySQLField::setOrgTable(const std::string& value) { m_orgTable = value; }
void cbMySQLField::setDB(const std::string& value) { m_db = value; }
void cbMySQLField::setCatalog(const std::string& value) { m_catalog = value; }
void cbMySQLField::setDef(const std::string& value) { m_def = value; }
void cbMySQLField::setCharsetnr(int32_t value) { m_charsetnr = value; }
void cbMySQLField::setLength(int32_t value) { m_length = value; }
void cbMySQLField::setFlags(int32_t value) { m_flags = value; }
void cbMySQLField::setDecimals(int32_t value) { m_decimals = value; }
void cbMySQLField::setDataType(int32_t value) { m_data_type = value; }

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

cbMySQLType cbMySQLCell::getType() { return m_type; }

std::vector<cbMySQLCell*> __luaPackedCellAsVec(cbMySQLCell* v) {
  if (v)
    return {v};
  else
    return {};
}

std::vector<std::string> __luaPackedStringAsVec() { return {}; }
