/**
 * @file cbComputeGraph.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The compute graph(DAG), a prepared graph for task flow to
 * execute. It works as a state machine.
 * 1. Shared memory
 * 2. Virtual table manager
 * 3. Node's inputs and outputs register.
 * 4. async management.
 * @version 0.1
 * @date 2022-11-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __SERVER_CB_COMPUTE_GRAPH_HPP_
#define __SERVER_CB_COMPUTE_GRAPH_HPP_

#include "cbOperator.hpp"

namespace graph {

/**
 * @brief
 *
 */
enum class nodeType : uint32_t {
  Leaf = 1,
  Output = 2,
  Operator = 3,
};

/**
 * @brief
 *
 */
struct cbGraphSharedMem {};

/**
 * @brief
 *
 */
struct cbGraphSharedLuaStack {};

/**
 * @brief
 *
 */
struct cbNode {};

/**
 * @brief
 *
 */
class cbComputeGraph {
 public:
  void initLuaStack();

 private:
  cbGraphSharedMem m_sharedMem;
  cbGraphSharedLuaStack m_sharedLuaStack;
};

};  // namespace graph

#endif  //!__SERVER_CB_COMPUTE_GRAPH_HPP_