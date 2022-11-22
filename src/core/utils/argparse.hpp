/*!
 *@author   chenghua.wang
 *@brief    A simple approach for arg-parse.
 * */

#ifndef H_ARGPARSE_
#define H_ARGPARSE_

///< For windows.
#if _MSC_VER > 1000
#pragma once
#endif  //! _MSC_VER > 1000, enable once pre.

#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <vector>

namespace argp {
using namespace std;

enum class arg_type {
  t_none = 0,  ///< for no value.
  t_int,
  t_uint,
  t_float,
  t_double,
  t_string,
  t_object
};

struct arg_payload_obj {
  arg_payload_obj() = default;
  arg_payload_obj(arg_type type, bool is_necessary) : _type(type), _is_necessary(is_necessary){};
  arg_type _type;
  string _name;
  string _s_name;
  bool _is_empty = true;
  bool _is_necessary;
};

template<typename T>
struct arg_payload : public arg_payload_obj {
  arg_payload() = default;
  arg_payload(arg_type type, bool is_necessary, T default_v)
      : arg_payload_obj(type, is_necessary), _default_v(default_v){};

  T _default_v;
  T _setted_v;
};

class parser {
 public:
  parser() = default;
  ~parser() {
    for (auto& item : m_k_v) { delete item.second; }
  };

 public:
  template<typename T>
  void add_arg(const string& name, const string& s_name, arg_type type, T default_v,
               bool is_necessary) {
    auto ptr = new arg_payload<T>(type, is_necessary, default_v);
    ptr->_setted_v = default_v;
    ptr->_name = name.substr(2, name.length() - 2);
    ptr->_s_name = s_name.substr(1, name.length() - 1);
    _add_arg_(name, ptr);
  }

  template<typename T>
  void add_arg(const string& name, const string& s_name, arg_type type, bool is_necessary) {
    auto ptr = new arg_payload<T>();
    ptr->_type = type;
    ptr->_is_necessary = is_necessary;
    ptr->_name = name.substr(2, name.length() - 2);
    ptr->_s_name = s_name.substr(1, name.length() - 1);
    _add_arg_(name, ptr);
  }

  /*!
   *@brief This function for no value flags.
   */
  void add_arg(const string& name, const string& s_name, bool is_necessary) {
    auto ptr = new arg_payload<bool>(arg_type::t_none, is_necessary, true);
    ptr->_name = name.substr(2, name.length() - 2);
    ptr->_s_name = s_name.substr(1, name.length() - 1);
    ptr->_setted_v = true;
    _add_arg_(name, ptr);
  }

  template<typename T>
  void _add_arg_(const string& name, arg_payload<T>* arg_ptr) {
    m_k_v[name.substr(2, name.length() - 2)] = arg_ptr;
  }

  template<typename T>
  T& get_item(const string& key) {
    /* only full name support */
    if (!m_parsed) { cout << "[Warn] You should call .parse function before get_item\n"; }

    auto ptr = m_k_v[key];
    auto ptr_c = (arg_payload<T>*)ptr;
    return ptr_c->_setted_v;
  }

  bool has_item(const string& key) {
    /* only full name support */
    return !m_k_v[key]->_is_empty;
  }

  virtual bool parse(int argc, char** args) {
    m_current_prog = string(args[0]);
    for (int i = 0; i < argc; ++i) { m_visited.push_back(false); }
    m_visited[0] = true;
    for (auto& item : m_k_v) {
      string tmp_name = "--" + item.second->_name;
      string tmp_s_name = "-" + item.second->_s_name;
      for (int i = 1; i < argc;) {
        if ((!strcmp(tmp_name.c_str(), args[i])) || (!strcmp(tmp_s_name.c_str(), args[i]))) {
          item.second->_is_empty = false;
          arg_type tmp_type = item.second->_type;
          m_visited[i] = true;
          if (tmp_type == arg_type::t_none) {
          } else if (tmp_type == arg_type::t_float) {
            auto ptr = item.second;
            auto ptr_c = (arg_payload<float>*)ptr;
            m_visited[i + 1] = true;
            ptr_c->_setted_v = atof(args[++i]);
          } else if (tmp_type == arg_type::t_double) {
            auto ptr = item.second;
            auto ptr_c = (arg_payload<double>*)ptr;
            m_visited[i + 1] = true;
            ptr_c->_setted_v = (double)atof(args[++i]);
          } else if (tmp_type == arg_type::t_int) {
            auto ptr = item.second;
            auto ptr_c = (arg_payload<int>*)ptr;
            m_visited[i + 1] = true;
            ptr_c->_setted_v = atoi(args[++i]);
          } else if (tmp_type == arg_type::t_uint) {
            auto ptr = item.second;
            auto ptr_c = (arg_payload<unsigned int>*)ptr;
            m_visited[i + 1] = true;
            ptr_c->_setted_v = (unsigned int)atoi(args[++i]);
          } else if (tmp_type == arg_type::t_string) {
            auto ptr = item.second;
            auto ptr_c = (arg_payload<string>*)ptr;
            m_visited[i + 1] = true;
            ptr_c->_setted_v = string(args[++i]);
          }
        }
        ++i;
      }
    }

    if (!m_visited.empty()) {
      int cnt = 0;
      for (auto idx : m_visited) {
        if (!idx) { m_others.push_back(string(args[cnt])); }
        cnt++;
      }
    }

    if (!m_others.empty()) {
      cout << "[Warn] Found other args:\n";
      for (auto& item : m_others) { cout << item << "\n"; }
    }

    m_parsed = true;
    return true;
  }

  virtual void show_all_parsed() {
    show_all_defined();
    // show others undefined or no-name value.
  }

  virtual void show_all_defined() {
    for (auto& item : m_k_v) {
      switch (item.second->_type) {
        case arg_type::t_int: {
          auto ptr = (arg_payload<int>*)item.second;
          cout << "--" << ptr->_name << " -" << ptr->_s_name << " ";
          cout << "Defualt is: " << ptr->_default_v << " ";
          cout << "Setted value is: " << ptr->_setted_v << endl;
        } break;
        case arg_type::t_uint: {
          auto ptr = (arg_payload<unsigned int>*)item.second;
          cout << "--" << ptr->_name << " -" << ptr->_s_name << " ";
          cout << "Defualt is: " << ptr->_default_v << " ";
          cout << "Setted value is: " << ptr->_setted_v << endl;
        } break;
        case arg_type::t_float: {
          auto ptr = (arg_payload<float>*)item.second;
          cout << "--" << ptr->_name << " -" << ptr->_s_name << " ";
          cout << "Defualt is: " << ptr->_default_v << " ";
          cout << "Setted value is: " << ptr->_setted_v << endl;
        } break;
        case arg_type::t_double: {
          auto ptr = (arg_payload<double>*)item.second;
          cout << "--" << ptr->_name << " -" << ptr->_s_name << " ";
          cout << "Defualt is: " << ptr->_default_v << " ";
          cout << "Setted value is: " << ptr->_setted_v << endl;
        } break;
        case arg_type::t_string: {
          auto ptr = (arg_payload<string>*)item.second;
          cout << "--" << ptr->_name << " -" << ptr->_s_name << " ";
          cout << "Defualt is: " << ptr->_default_v << " ";
          cout << "Setted value is: " << ptr->_setted_v << endl;
        } break;
        case arg_type::t_none: {
          auto ptr = (arg_payload<string>*)item.second;
          cout << "--" << ptr->_name << " -" << ptr->_s_name << " ";
          cout << "is a no value flag" << endl;
        } break;
        default: break;
      }
    }
  }

 protected:
  bool m_parsed = false;
  map<string, arg_payload_obj*> m_k_v;
  string m_current_prog;
  vector<bool> m_visited;
  vector<string> m_others;
};

}  // namespace argp

#endif  //! H_ARGPARSE
