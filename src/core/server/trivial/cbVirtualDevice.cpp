#include "cbVirtualDevice.hpp"
#include <workflow/WFOperator.h>

namespace trivial {
void cbVirtualDevice::reloadConnection(const std::string& _port, const std::string& _host,
                                       const std::string& _userName, const std::string& _passWord,
                                       const std::string& _charSet,
                                       const std::string& _dataBaseName) {
  port = _port;
  host = _host;
  usrName = _userName;
  passWord = _passWord;
  charSet = _charSet;
  dataBaseName = _dataBaseName;
}

void cbMySqlDevice::updateUrl() {
  std::stringstream ss;
  ss << "mysql://" << usrName << ":" << passWord << "@" << host << ":" << port << "/"
     << dataBaseName;
  if (!charSet.empty()) {
    ss << "?character_set=" << charSet << "&character_set_results=" << charSet;
  }
  Url = ss.str();
  m_conn->deinit();
  m_conn->init(Url);
}

int32_t cbMySqlDevice::getIdx() { return m_idx; }

WFMySQLTask* cbMySqlDevice::query(const std::string& q, const mysql_callback& callback_func) {
  return m_conn->create_query_task(q, callback_func);
}

std::vector<WFMySQLTask*> cbMySqlDevice::sequentialQuery(
    const std::vector<std::string>& q, const std::vector<mysql_callback>& callback_func) {
  if (q.size() != callback_func.size()) {
    fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
               "[ Error ] sequential task's vector size is not same. Get query size={}, but "
               "callback_func size={}",
               q.size(), callback_func.size());
  }
  size_t len_size = q.size();
  std::vector<WFMySQLTask*> ans;
  for (size_t i = 0; i < len_size; ++i) {
    ans.push_back(m_conn->create_query_task(q[i], callback_func[i]));
  }
  return ans;
}

void cbMySqlDevice::execMain(WFMySQLTask* t) { t->start(); }

void cbMySqlDevice::execMain(const std::vector<WFMySQLTask*>& t) {
  auto& generalTask = (*t[0]) > t[1];
  size_t len_size = t.size();
  for (size_t i = 2; i < len_size; ++i) { generalTask.push_back(t[i]); }
  generalTask.start();
}

void cbRedisDevice::updateUrl() {
  // if no ssl: redis://:password@host:port/dbnum?query#fragment
  // if ssl: rediss://:password@host:port/dbnum?query#fragment
  std::stringstream ss;
  if (m_isSSL) {
    ss << "rediss://:";
  } else {
    ss << "redis://:";
  }
  ss << passWord << "@" << host << ":" << port << "/" << std::to_string(m_dbnum);
  //  << "?query#fragment";
  Url = ss.str();
}

WFRedisTask* cbRedisDevice::set(const std::vector<std::string>& params,
                                const redis_callback& callback_func, void* usrData,
                                int32_t retryTimes) {
  WFRedisTask* task = WFTaskFactory::create_redis_task(Url, retryTimes, callback_func);
  protocol::RedisRequest* req = task->get_req();
  req->set_request("SET", params);
  task->user_data = usrData;
  return task;
}

WFRedisTask* cbRedisDevice::get(const std::vector<std::string>& params,
                                const redis_callback& callback_func, void* usrData,
                                int32_t retryTimes) {
  WFRedisTask* task = WFTaskFactory::create_redis_task(Url, retryTimes, callback_func);
  protocol::RedisRequest* req = task->get_req();
  req->set_request("GET", params);
  task->user_data = usrData;
  return task;
}

WFRedisTask* cbRedisDevice::exists(const std::vector<std::string>& params,
                                   const redis_callback& callback_func, void* usrData,
                                   int32_t retryTimes) {
  WFRedisTask* task = WFTaskFactory::create_redis_task(Url, retryTimes, callback_func);
  protocol::RedisRequest* req = task->get_req();
  req->set_request("EXISTS", params);
  task->user_data = usrData;
  return task;
}

void cbRedisDevice::execMain(WFRedisTask* task) { task->start(); }

void cbKafkaDevice::updateUrl() {
  // TODO
}

int32_t cbVirtualDeviceManager::m_numsMySql = 0;
int32_t cbVirtualDeviceManager::m_numsRedis = 0;
int32_t cbVirtualDeviceManager::m_numsKafka = 0;

void cbVirtualDeviceManager::addMySqlDevice(cbMySqlDevice* vd) {
  m_mySqlPool[m_numsMySql++] = {vd, true};
}

void cbVirtualDeviceManager::addRedisDevice(cbRedisDevice* vd) {
  m_redisPool[m_numsRedis++] = {vd, true};
}

void cbVirtualDeviceManager::addKafkaDevice(cbKafkaDevice* vd) {
  m_kafkaPool[m_numsKafka++] = {vd, true};
}

void cbVirtualDeviceManager::removeMySqlDevice(int32_t idx) {
  delete m_mySqlPool[idx].first;
  m_mySqlPool[idx].second = false;
}

void cbVirtualDeviceManager::removeRedisDevice(int32_t idx) {
  delete m_redisPool[idx].first;
  m_redisPool[idx].second = false;
}

void cbVirtualDeviceManager::removeKafkaDevice(int32_t idx) {
  delete m_kafkaPool[idx].first;
  m_kafkaPool[idx].second = false;
}

cbMySqlDevice* cbVirtualDeviceManager::getMySqlDevice(int32_t idx) {
  if (m_mySqlPool[idx].second) { return m_mySqlPool[idx].first; }
  return nullptr;
}

cbRedisDevice* cbVirtualDeviceManager::getRedisDevice(int32_t idx) {
  if (m_redisPool[idx].second) { return m_redisPool[idx].first; }
  return nullptr;
}

cbKafkaDevice* cbVirtualDeviceManager::getKafkaDevice(int32_t idx) {
  if (m_kafkaPool[idx].second) { return m_kafkaPool[idx].first; }
  return nullptr;
}

}  // namespace trivial