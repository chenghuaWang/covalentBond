#include <workflow/WFFacilities.h>
#include "trivial/cbVirtualDevice.hpp"

using namespace protocol;

static WFFacilities::WaitGroup wait_group(1);

int main() {
  WFMySQLConnection conn(1);
  conn.init("*");
  WFMySQLTask* t1;
  // test transaction
  const char* query = "select * from runoob_tbl;";
  t1 = conn.create_query_task(query, [](WFMySQLTask* task) {
    if (task->get_state() != WFT_STATE_SUCCESS) {
      fprintf(stderr, "task error = %d\n", task->get_error());
      return;
    }
    MySQLResultCursor cursor(task->get_resp());

    // step-2. 判断回复包其他状态
    if (task->get_resp()->get_packet_type() == MYSQL_PACKET_ERROR) {
      fprintf(stderr, "ERROR. error_code=%d %s\n", task->get_resp()->get_error_code(),
              task->get_resp()->get_error_msg().c_str());
    }
    // step-3. 遍历结果集
    // step-4. 判断结果集状态
    if (cursor.get_cursor_status() == MYSQL_STATUS_OK) {
      // step-5. MYSQL_STATUS_OK结果集的基本内容
      fprintf(stderr, "OK. %llu rows affected. %d warnings. insert_id=%llu.\n",
              cursor.get_affected_rows(), cursor.get_warnings(), cursor.get_insert_id());
    } else if (cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT) {
      fprintf(stderr, "field_count=%u rows_count=%u ", cursor.get_field_count(),
              cursor.get_rows_count());

      // step-6. 读取每个fields。这是个nocopy api
      const MySQLField* const* fields = cursor.fetch_fields();
      for (int i = 0; i < cursor.get_field_count(); i++) {
        fprintf(stderr, "db=%s table=%s name[%s] type[%s]\n", fields[i]->get_db().c_str(),
                fields[i]->get_table().c_str(), fields[i]->get_name().c_str(),
                datatype2str(fields[i]->get_data_type()));
      }

      // step-8. 把所有行读出，也可以while (cursor.fetch_row(map/vector)) 按step-7拿每一行
      std::vector<std::vector<MySQLCell>> rows;

      cursor.fetch_all(rows);
      for (unsigned int j = 0; j < rows.size(); j++) {
        // step-12. 具体每个cell的读取
        for (unsigned int i = 0; i < rows[j].size(); i++) {
          fprintf(stderr, "[%s][%s]", fields[i]->get_name().c_str(),
                  datatype2str(rows[j][i].get_data_type()));
          // step-12. 判断具体类型is_string()和转换具体类型as_string()
          if (rows[j][i].is_string()) {
            std::string res = rows[j][i].as_string();
            fprintf(stderr, "[%s]\n", res.c_str());
          } else if (rows[j][i].is_int()) {
            fprintf(stderr, "[%d]\n", rows[j][i].as_int());
          }  // else if ...
        }
      }
    }
    // step-10. 拿下一个结果集
    wait_group.done();
    return;
  });
  t1->start();
  wait_group.wait();
  return 0;
}