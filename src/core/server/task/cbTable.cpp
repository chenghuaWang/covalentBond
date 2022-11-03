#include "cbTable.hpp"

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
    cursor->fetch_all(m_data);  ///! with copy.
    m_shape[0] = cursor->get_rows_count();
    m_shape[1] = static_cast<int32_t>(m_data[0].size());
  }
}

protocol::MySQLCell* cbVirtualSharedTable::at(int32_t i, int32_t j) { return &m_data[i][j]; }

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

std::vector<std::vector<protocol::MySQLCell*>>& cbVirtualTable::getData() { return m_data; }

protocol::MySQLCell* cbVirtualTable::at(int32_t i, int32_t j) { return m_data[i][j]; }

std::string cbVirtualTable::colNameAt(int32_t i) { return m_info[i]->get_name(); }

std::string cbVirtualTable::colTypeAt(int32_t i) {
  return datatype2str(m_info[i]->get_data_type());
}

void mapShared2Virtual(cbVirtualSharedTable* sharedT, cbVirtualTable* virtualT) {
  virtualT->resetShape(sharedT->getShape());
  int32_t row = virtualT->getShape()[0];
  int32_t col = virtualT->getShape()[1];

  auto p = virtualT->getInfo();
  p = sharedT->getInfo();

  for (int32_t i = 0; i < row; ++i) {
    for (int32_t j = 0; j < col; ++j) {
      auto p = virtualT->at(i, j);
      p = sharedT->at(i, j);
    }
  }
}