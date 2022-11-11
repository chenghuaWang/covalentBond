/**
 * @file cbLRUCache.hpp
 * @author caibo Feng (3418552929@qq.com)
 * @brief The impl of LRU cache.
 * @version 0.1
 * @date 2022-11-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <map>
#include <list>
#include "../../pch.hpp"

/**
 * @brief
 * Macro define of loop in cache list
 */
#define find_list for (auto i = m_cacheList.begin(); i != m_cacheList.end(); i++)

namespace cb {
/**
 * @brief Elements of list
 *
 */
struct listNode {
  std::string key;
};

typedef listNode node;

/**
 * @brief  We used LRU alogrithm to implement our cache
 *
 * @tparam V You can set any type of value and key only
 * support string
 */
template<typename V>
class cbLRUCache {
 public:
  /**
   * @brief Construct a new cb Least Recently Use Cache object
   *
   * @param max_size Maximum sixe of cache
   */
  cbLRUCache(int32_t max_size) : m_maxSize(max_size), m_sizeCounter(0), m_bufNode(nullptr) {}

  /**
   * @brief Destroy the cb Least Recently Use Cache object
   *
   */
  ~cbLRUCache() { delete m_bufNode; }

  /**
   * @brief Push key and value into cache. If key has been existed,
   * the corresponding value will be update
   *
   * @param key key of cache map
   * @param value value of cache map
   */
  void push(std::string key, V value) {
    find_list {
      if (i->key == key) {
        m_cacheMap[key] = value;
        this->update(key);
        return;
      }
    }

    m_sizeCounter++;
    if (m_sizeCounter > m_maxSize) {
      m_sizeCounter = m_maxSize;
      this->eraseTail();
    }
    m_cacheMap.insert({key, value});
    this->insertTop(key);
  }

  /**
   * @brief Get the value of corresponding key of input,
   * if key doesn't exist, it will return null pointer.
   *
   * @param key key of cache map
   * @return V* the pointer of type V
   */
  V* get(const std::string& key) {
    if (m_cacheMap.count(key)) {
      this->update(key);
      V* exit = nullptr;
      exit = &m_cacheMap[key];
      // this->__loop_map();
      return exit;
    }
    V* not_exit = nullptr;
    return not_exit;
  }

 private:
  /**
   * @brief delete the last element of list
   *
   */
  void eraseTail() {
    m_bufNode = &m_cacheList.back();
    m_bufKey = m_bufNode->key;
    auto buf_ptr = m_cacheMap.find(m_bufKey);
    m_cacheMap.erase(buf_ptr);
    m_cacheList.pop_back();
  }

  /**
   * @brief move the node has been used recently to
   * the top of cache list
   *
   * @param key key of cache map
   */
  void update(const std::string& key) {
    find_list {
      if (i->key == key) {
        m_cacheList.erase(i);
        break;
      }
    }
    this->insertTop(key);
  }

  /**
   * @brief transit the node choosen to the top of list
   *
   * @param key key of cache map
   */
  void insertTop(const std::string& key) {
    m_bufNode = new (node);
    m_bufNode->key = key;
    m_cacheList.push_front(*m_bufNode);
    delete (m_bufNode);
    m_bufNode = nullptr;
  }

  /**
   * @brief loop in cachae list and map
   *
   */
  [[deprecated("For debug only")]] void __loop_map() {
    std::string bs;
    int32_t n = 0;
    for (auto i = m_cacheList.begin(); i != m_cacheList.end(); i++) {
      bs = i->key;
      std::cout << "The" << n << "th element" << bs << " : " << m_cacheMap[bs] << std::endl;
      n++;
    }
  }

 private:
  int32_t m_maxSize;
  int32_t m_sizeCounter;
  node* m_bufNode;
  std::string m_bufKey;
  std::list<node> m_cacheList;
  std::map<std::string, V> m_cacheMap;
};
}  // namespace cb
