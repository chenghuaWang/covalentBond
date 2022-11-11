#include <cstddef>
#include <cstdint>
#include <map>
#include <list>
#include <iostream>
#include <string>
#include "../../pch.hpp"

namespace cb {
/**
 * @brief Elements of list
 *
 */
struct listNode {
  std::string key;
};

/**
 * @brief  We used LRU alogrithm to implement our cache
 *
 * @tparam V You can set any type of value and key only
 * support string
 */
template<typename V>
class cbLRUCache {
 protected:
  typedef listNode node;
  /**
   * @brief
   * Macro define of loop in cache list
   */
#define find_list for (auto i = m_cache_list.begin(); i != m_cache_list.end(); i++)

 public:
  /**
   * @brief Construct a new cb Least Recently Use Cache object
   *
   * @param max_size Maximum sixe of cache
   */
  cbLRUCache(int32_t max_size) : m_max_size(max_size) {
    m_size_counter = 0;
    m_buf_node = nullptr;
  }
  /**
   * @brief Destroy the cb Least Recently Use Cache object
   *
   */
  ~cbLRUCache() {
    if (m_buf_node) delete m_buf_node;
  }

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
        m_cache_map[key] = value;
        this->update(key);
        return;
      }
    }

    m_size_counter++;
    if (m_size_counter > m_max_size) {
      m_size_counter = m_max_size;
      this->erase_tail();
    }
    m_cache_map.insert({key, value});
    this->insert_top(key);
  }

  /**
   * @brief Get the value of corresponding key of input,
   * if key doesn't exist, it will return null pointer.
   *
   * @param key key of cache map
   * @return V* the pointer of type V
   */
  V* get(const std::string& key) {
    if (m_cache_map.count(key)) {
      this->update(key);
      V* exit = nullptr;
      exit = &m_cache_map[key];
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
  void erase_tail() {
    m_buf_node = &m_cache_list.back();
    buf_key = m_buf_node->key;
    auto buf_ptr = m_cache_map.find(buf_key);
    m_cache_map.erase(buf_ptr);
    m_cache_list.pop_back();
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
        m_cache_list.erase(i);
        break;
      }
    }
    this->insert_top(key);
  }

  /**
   * @brief transit the node choosen to the top of list
   *
   * @param key key of cache map
   */
  void insert_top(const std::string& key) {
    m_buf_node = new (node);
    m_buf_node->key = key;
    m_cache_list.push_front(*m_buf_node);
    delete (m_buf_node);
    m_buf_node = nullptr;
  }

  /**
   * @brief loop in cachae list and map
   *
   */
  [[deprecated("For debug only")]] void __loop_map() {
    std::string bs;
    int32_t n = 0;
    for (auto i = m_cache_list.begin(); i != m_cache_list.end(); i++) {
      bs = i->key;
      std::cout << "第" << n << "个元素：" << bs << " : " << m_cache_map[bs] << std::endl;
      n++;
    }
  }

  int32_t m_max_size;
  int32_t m_size_counter;
  node* m_buf_node;
  std::string buf_key;
  std::list<node> m_cache_list;
  std::map<std::string, V> m_cache_map;
};
}  // namespace cb
