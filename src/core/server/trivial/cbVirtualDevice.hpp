/**
 * @file cbVirtualDevice.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief abstract virtual device. Provide MySql/Redis/Kafka warper.
 * All virtual device will handle the connection time and connection nums.
 * @version 0.1
 * @date 2022-11-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __SERVER_CB_VIRTUAL_DEVICE_HPP_
#define __SERVER_CB_VIRTUAL_DEVICE_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../../pch.hpp"

#include <workflow/MySQLResult.h>
#include <workflow/WFMySQLConnection.h>
#include <workflow/WFRedisServer.h>
#include <workflow/WFTaskFactory.h>

using mysql_callback = std::function<void(WFMySQLTask*)>;
using redis_callback = std::function<void(WFRedisTask*)>;

namespace trivial {
/**
 * @brief
 *
 */
enum class virtualDeviceType {
  Redis = 0,
  MySql = 1,
  Kafka = 2,
  HttpDefine = 3,
};

struct cbVirtualDevice {
  cbVirtualDevice() = default;
  cbVirtualDevice(const std::string& _port, const std::string& _host, const std::string& _userName,
                  const std::string& _passWord, const std::string& _charSet,
                  const std::string _dataBaseName, const virtualDeviceType& v)
      : deviceType(v),
        port(_port),
        host(_host),
        usrName(_userName),
        passWord(_passWord),
        charSet(_charSet),
        dataBaseName(_dataBaseName) {}

  virtualDeviceType deviceType = virtualDeviceType::MySql;
  std::string port;
  std::string host;
  std::string usrName;
  std::string passWord;
  std::string charSet;
  std::string dataBaseName;

  std::string Url;

  /**
   * @brief
   *
   * @param _port
   * @param _host
   * @param _userName
   * @param _passWord
   * @param _charSet
   * @param _dataBaseName
   */
  void reloadConnection(const std::string& _port, const std::string& _host,
                        const std::string& _userName, const std::string& _passWord,
                        const std::string& _charSet, const std::string& _dataBaseName);

  /**
   * @brief
   *
   */
  virtual void updateUrl() = 0;
};

/**
 * @brief
 *
 */
struct cbMySqlDevice final : public cbVirtualDevice {
  ~cbMySqlDevice() {
    m_conn->deinit();
    delete m_conn;
  }
  cbMySqlDevice(int32_t _idx) : m_idx(_idx) { m_conn = new WFMySQLConnection(_idx); }
  cbMySqlDevice(int32_t _idx, const std::string& _port, const std::string& _host,
                const std::string& _userName, const std::string& _passWord,
                const std::string& _dataBaseName, const std::string& _charSet = "")
      : cbVirtualDevice(_port, _host, _userName, _passWord, _charSet, _dataBaseName,
                        virtualDeviceType::MySql),
        m_idx(_idx) {
    m_conn = new WFMySQLConnection(_idx);
    // In my implementation. The mysql client should give the url like this:
    // mysql://username:password@host:port/dbname?character_set=charset&character_set_results=charset
    // And I assume the charset of result and original database are same.
    std::stringstream ss;
    ss << "mysql://" << _userName << ":" << _passWord << "@" << _host << ":" << _port << "/"
       << _dataBaseName;
    if (!_charSet.empty()) {
      ss << "?character_set=" << _charSet << "&character_set_results=" << _charSet;
    }
    Url = ss.str();
    // Init the connection;
    m_conn->init(Url);
  }

  /**
   * @brief
   *
   */
  void updateUrl() override final;

  /**
   * @brief
   *
   * @param q
   * @param callback_func
   * @return WFMySQLTask*
   */
  WFMySQLTask* query(const std::string& q, const mysql_callback& callback_func = nullptr);

  /**
   * @brief
   *
   * @param q
   * @param callback_func
   * @return std::vector<WFMySQLTask*>
   */
  std::vector<WFMySQLTask*> sequentialQuery(const std::vector<std::string>& q,
                                            const std::vector<mysql_callback>& callback_func);
  /**
   * @brief User should not use this function. All task should be added to graph node in order to
   * execute all task that follows DAG flow. THIS FUNCTION JUST FOR TEST.
   *
   * @param t
   */
  static void execMain(WFMySQLTask* t);

  /**
   * @brief User should not use this function. All task should be added to graph node in order to
   * execute all task that follows DAG flow. THIS FUNCTION JUST FOR TEST.
   *
   * @param t
   */
  static void execMain(const std::vector<WFMySQLTask*>& t);

  /**
   * @brief Get the Idx object
   *
   * @return int32_t
   */
  int32_t getIdx();

 private:
  int32_t m_idx = 0;
  WFMySQLConnection* m_conn = nullptr;
};

/**
 * @brief
 *
 */
struct cbRedisDevice final : public cbVirtualDevice {
  ~cbRedisDevice() {}
  cbRedisDevice(int32_t idx) : m_idx(idx) {}
  cbRedisDevice(int32_t idx, const std::string& _port, const std::string& _host,
                const std::string& _userName, const std::string& _passWord, int32_t dbnum,
                bool isSsl = false)
      : cbVirtualDevice(_port, _host, _userName, _passWord, "", "", virtualDeviceType::Redis),
        m_isSSL(isSsl),
        m_dbnum(dbnum),
        m_idx(idx) {
    // if no ssl: redis://:password@host:port/dbnum?query#fragment
    // if ssl: rediss://:password@host:port/dbnum?query#fragment
    updateUrl();
  }

  void updateUrl() override final;

  /**
   * @brief
   *
   * @param params
   * @param callback_func
   * @param usrData
   * @param retryTimes
   * @return WFRedisTask*
   */
  WFRedisTask* set(const std::vector<std::string>& params,
                   const redis_callback& callback_func = nullptr, void* usrData = nullptr,
                   int32_t retryTimes = 3);

  /**
   * @brief
   *
   * @param params
   * @param callback_func
   * @param usrData
   * @param retryTimes
   * @return WFRedisTask*
   */
  WFRedisTask* get(const std::vector<std::string>& params,
                   const redis_callback& callback_func = nullptr, void* usrData = nullptr,
                   int32_t retryTimes = 3);

  /**
   * @brief
   *
   * @param params
   * @param callback_func
   * @param usrData
   * @param retryTimes
   * @return WFRedisTask*
   */
  WFRedisTask* exists(const std::vector<std::string>& params,
                      const redis_callback& callback_func = nullptr, void* usrData = nullptr,
                      int32_t retryTimes = 3);

  /**
   * @brief User should not use this function. All task should be added to graph node in order to
   * execute all task that follows DAG flow. THIS FUNCTION JUST FOR TEST.
   *
   * @param task
   */
  static void execMain(WFRedisTask* task);

 private:
  bool m_isSSL = false;
  int32_t m_dbnum = 0;
  int32_t m_idx = 0;
};

/**
 * @brief
 *
 */
struct cbKafkaDevice final : public cbVirtualDevice {
  void updateUrl() override final;
};

/**
 * @brief
 *
 */
class cbVirtualDeviceManager {
 public:
  cbVirtualDeviceManager() = default;
  cbVirtualDeviceManager(const cbVirtualDeviceManager& t) = delete;
  cbVirtualDeviceManager operator=(const cbVirtualDeviceManager& t) = delete;

  ~cbVirtualDeviceManager() {
    for (auto& item : m_mySqlPool) {
      if (item.second.second) { delete item.second.first; }
    }
    for (auto& item : m_redisPool) {
      if (item.second.second) { delete item.second.first; }
    }
    for (auto& item : m_kafkaPool) {
      if (item.second.second) { delete item.second.first; }
    }
  }

  void addMySqlDevice(cbMySqlDevice* vd);
  void addRedisDevice(cbRedisDevice* vd);
  void addKafkaDevice(cbKafkaDevice* vd);

  void removeMySqlDevice(int32_t idx);
  void removeRedisDevice(int32_t idx);
  void removeKafkaDevice(int32_t idx);

  cbMySqlDevice* getMySqlDevice(int32_t idx);
  cbRedisDevice* getRedisDevice(int32_t idx);
  cbKafkaDevice* getKafkaDevice(int32_t idx);

  static int32_t m_numsMySql;
  static int32_t m_numsRedis;
  static int32_t m_numsKafka;

 private:
  std::map<int32_t, std::pair<cbMySqlDevice*, bool>> m_mySqlPool;  ///! index, {pointer, visibility}
  std::map<int32_t, std::pair<cbRedisDevice*, bool>> m_redisPool;  ///! index, {pointer, visibility}
  std::map<int32_t, std::pair<cbKafkaDevice*, bool>> m_kafkaPool;  ///! index, {pointer, visibility}
};

};  // namespace trivial

#endif  //! __SERVER_CB_VIRTUAL_DEVICE_HPP_