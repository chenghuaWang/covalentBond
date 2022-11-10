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
#include <workflow/WFGraphTask.h>
#include <workflow/WFFacilities.h>

typedef std::function<void(WFGraphTask*)> graph_callback;

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
 * @brief The shared memory of compute graph. Include 2 basic components.
 * 1. The Virtual Shared Table
 * 2. The Shared memory pool. Worked as a vector.
 */
struct cbGraphSharedMem {
 public:
  cbGraphSharedMem() = default;
  cbGraphSharedMem(cbGraphSharedMem& rhs) = delete;
  cbGraphSharedMem(const cbGraphSharedMem& rhs) = delete;
  cbGraphSharedMem operator=(cbGraphSharedMem& rhs) = delete;
  cbGraphSharedMem operator=(const cbGraphSharedMem& rhs) = delete;
  ~cbGraphSharedMem();

  void push(cbVirtualSharedTable* v);
  void push(cbMySQLCell* v);

  size_t getMemUsed();
  int32_t getCellNum();

 private:
  std::vector<cbVirtualSharedTable*> m_dataFromDevice;
  std::vector<cbMySQLCell*> m_dataPool;
};

/**
 * @brief
 *
 */
struct cbGraphSharedLuaStack {
  // TODO
};

/**
 * @brief
 *
 */
struct cbNode {
  virtual ~cbNode() = 0;  // TODO maybe bugs !!!
  cbNode(const nodeType& nt);
  void PointTo(cbNode* ptr);

  virtual void* generateTask() = 0;

  static WFMySQLTask* asSQLTask(void* metaTask);
  static WFGoTask* asGoTask(void* metaTask);

  nodeType nodeT;
  void* task = nullptr;
  cbNode* nextNode = nullptr;
};

/**
 * @brief
 *
 */
struct cbVirtualDeviceNode final : public cbNode {
  ~cbVirtualDeviceNode() override final;
  cbVirtualDeviceNode();
  void* generateTask() override final;
};

// operator Node;

/**
 * @brief
 *
 */
class cbComputeGraph {
 public:
  cbComputeGraph(int32_t idx);
  ~cbComputeGraph();

  bool isDAG();
  bool isSingleOutput();

  // TODO multi initialize function impl.
  cbMySQLCell* createCell();

  void registerNode(cbNode* node);

  int32_t getId();

  WFGraphTask* generateGraphTask(const graph_callback& func);

  static void execMain(WFGraphTask* task, cbComputeGraph* graph);

 private:
  int32_t m_idx = 0;
  std::vector<cbNode*> m_nodes;
  cbGraphSharedMem* m_sharedMem;
  cbGraphSharedLuaStack* m_sharedLuaStack;
  // For wait the graph done.
  WFFacilities::WaitGroup m_waitGroup;
};

};  // namespace graph

#endif  //!__SERVER_CB_COMPUTE_GRAPH_HPP_