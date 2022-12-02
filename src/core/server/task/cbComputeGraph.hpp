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

#include <workflow/WFGraphTask.h>
#include <workflow/WFFacilities.h>
#include <workflow/WFTaskFactory.h>

#include "luaEngine.hpp"
#include "cbOperator.hpp"
#include "trivial/cbVirtualDevice.hpp"

typedef std::function<void(WFGraphTask*)> graph_callback;

namespace cb {
namespace graph {

class cbComputeGraph;

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
  void push(cbMySQLField* v);
  void setOutStruct(const cbShape<2>& shape, cbMySQLField** info);

  size_t getMemUsed();
  int32_t getCellNum();
  cbOutputTableStruct* getOutStruct();

  void clear();

 private:
  cbOutputTableStruct* m_outStruct = nullptr;
  std::vector<cbVirtualSharedTable*> m_dataFromDevice;
  std::vector<cbMySQLCell*> m_dataPool;
  std::vector<cbMySQLField*> m_fields;
};

/**
 * @brief
 *
 */
struct cbGraphSharedLuaStack {
  cbGraphSharedLuaStack() = default;
  cbGraphSharedLuaStack(const cbGraphSharedLuaStack&) = delete;
  cbGraphSharedLuaStack operator=(const cbGraphSharedLuaStack&) = delete;

  /**
   * @brief execute the script file from disk directly.
   *
   * @param filePath string.
   */
  void execScriptFile(const std::string& filePath);

  /**
   * @brief execute the hard coded script.
   *
   * @param script
   */
  void execScript(const std::string& script);

  /**
   * @brief get the lua state.
   *
   * @return luaJitThread&
   */
  luaJitThread& get();

 private:
  luaJitThread m_lua;
};

/**
 * @brief
 *
 */
struct cbNode {
  virtual ~cbNode(){};  // TODO maybe bugs !!!
  cbNode(const nodeType& nt);
  void PointTo(cbNode* ptr);

  virtual void* generateTask() = 0;

  static WFMySQLTask* asSQLTask(void* metaTask);
  static WFGoTask* asGoTask(void* metaTask);

  // The output.
  cbOpIO io;

  // others
  bool isFinalOutput = false;
  bool visited = false;
  nodeType nodeT;
  void* task = nullptr;
  cbNode* nextNode = nullptr;
  cbComputeGraph* graph = nullptr;
};

/**
 * @brief This node include virtual device infomation and
 * perform the final operation of how to get the data from
 * remote sql database.
 * @note This node just support SQL for now. But it's quite
 * easy to extened to other relationship database.
 *
 */
struct cbVirtualDeviceNode final : public cbNode {
  friend cbComputeGraph;

  ~cbVirtualDeviceNode() override final;
  cbVirtualDeviceNode();

  /**
   * @brief generate a SQL wf task for now.
   *
   * @return void* WFMySQLTask* actually.
   */
  void* generateTask() override final;

  /**
   * @brief Add a String type sql sentence to this node. Waiting to be execute.
   *
   * @param q string
   */
  void addQuery(const std::string& q);

 private:
  void setMySQLDevice(trivial::cbMySqlDevice* device = nullptr);
  trivial::cbMySqlDevice* m_device;
  std::vector<std::string> m_queries;
};

struct cbRedisCachingNode final : public cbNode {
  friend cbComputeGraph;

  ~cbRedisCachingNode() override final;
  cbRedisCachingNode() = delete;
  cbRedisCachingNode(int32_t idx);
  cbRedisCachingNode(const cbRedisCachingNode& v) = delete;

  cbRedisCachingNode operator=(const cbRedisCachingNode& v) = delete;

  void* generateTask() override final;

  WFRedisTask* _generateSetTask(const std::vector<std::string>& params,
                                const redis_callback& callback_func = nullptr,
                                void* usrData = nullptr, int32_t retryTimes = 3);

 private:
  void setRedisDevice(trivial::cbRedisDevice* device = nullptr);
  int32_t m_idx;
  trivial::cbRedisDevice* m_device = nullptr;
};

/**
 * @brief A operator node. Generate all go task. Then pass the output
 * to the next node's inputs.
 *
 */
struct cbOperatorNode : public cbNode {
  friend cbComputeGraph;

  ~cbOperatorNode() override;
  cbOperatorNode(baseOp* op);

  /**
   * @brief
   *
   * @return void*
   */
  void* generateTask() override;

  /**
   * @brief
   *
   * @param funcPtr The functions belong to lua.
   */
  void overload(sol::function& funcPtr);

  baseOp* Op;
};

// operator Node;

/**
 * @brief
 *
 */
class cbComputeGraph {
 public:
  friend cbVirtualDeviceNode;
  friend cbRedisCachingNode;
  friend cbOperatorNode;

  cbComputeGraph(int32_t idx);
  ~cbComputeGraph();

  /**
   * @brief To judge the Graph is DAG or not.
   *
   * @return true
   * @return false
   */
  bool isDAG();

  /**
   * @brief To judge the graph has single output or not.
   *
   * @return true
   * @return false
   */
  bool isSingleOutput();

  // create cells belongs to this graph.
  /**
   * @brief Create a Cell object
   *
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell();

  /**
   * @brief Create a Cell object
   *
   * @param value int
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell(int value);

  /**
   * @brief Create a Cell object
   *
   * @param value float
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell(float value);

  /**
   * @brief Create a Cell object
   *
   * @param value double
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell(double value);

  /**
   * @brief Create a Cell object
   *
   * @param value unsigned long long
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell(unsigned long long value);

  /**
   * @brief Create a Cell object
   *
   * @param value string
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell(const std::string& value);

  /**
   * @brief Create a Cell object
   *
   * @param value string
   * @param t the type. such as String, Date, DateTime.
   * @return cbMySQLCell*
   */
  cbMySQLCell* createCell(const std::string& value, const cbMySQLType& t);

  /**
   * @brief create nodes belongs to this graph.
   *
   * @param idx
   * @return cbVirtualDeviceNode*
   */
  cbVirtualDeviceNode* createVirtualDeviceNode(int32_t idx);  ///! for sql only, now.

  /**
   * @brief Create a Redis Caching Node object
   *
   * @param idx
   * @return cbRedisCachingNode*
   */
  cbRedisCachingNode* createRedisCachingNode(int32_t idx);

  /**
   * @brief Create a Combine Node object
   *
   * @param keys
   * @return cbOperatorNode*
   */
  cbOperatorNode* createCombineNode(const std::vector<std::string>& keys);

  /**
   * @brief Set the Virtual Device Manager object
   *
   * @param virtualDeviceM
   */
  void setVirtualDeviceManager(trivial::cbVirtualDeviceManager* virtualDeviceM);

  /**
   * @brief register a node. Not used by lua binding.
   *
   * @param node
   */
  void registerNode(cbNode* node);

  /**
   * @brief Get the Id object
   *
   * @return int32_t
   */
  int32_t getId();

  /**
   * @brief Create the graph task.
   *
   * @param func
   * @return WFGraphTask*
   */
  WFGraphTask* generateGraphTask(const graph_callback& func = nullptr);

  /**
   * @brief Execute the graph task.
   *
   * @param task
   * @param graph
   */
  static void execMain(WFGraphTask* task, cbComputeGraph* graph);

  /**
   * @brief execute the script from file.
   *
   * @param filePath
   */
  void execScriptFile(const std::string& filePath);

  /**
   * @brief execute the script
   *
   * @param script
   */
  void execScript(const std::string& script);

  /**
   * @brief The cache server node. Lua binding in.
   *
   * @param v the redis caching server node.
   */
  void addCacheServer(cbRedisCachingNode* v);

  /**
   * @brief Get the Output object
   *
   * @return cbOutputTableStruct*
   */
  cbOutputTableStruct* getOutput();

 private:
  int32_t m_idx = 0;
  cbOpIO io;
  std::vector<cbNode*> m_nodes;
  cbRedisCachingNode* m_cacheNode = nullptr;
  cbGraphSharedMem* m_sharedMem = nullptr;
  cbGraphSharedLuaStack* m_sharedLuaStack = nullptr;
  // Have the state of all Virtual device
  trivial::cbVirtualDeviceManager* m_virtualDevice = nullptr;
};

};  // namespace graph
}  // namespace cb

#endif  //!__SERVER_CB_COMPUTE_GRAPH_HPP_