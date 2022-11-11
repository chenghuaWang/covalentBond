#include "cbComputeGraph.hpp"
#include <workflow/WFGraphTask.h>

namespace graph {

cbGraphSharedMem::~cbGraphSharedMem() {
  for (auto item : m_dataFromDevice) { delete item; }
  for (auto item : m_dataPool) { delete item; }
}

void cbGraphSharedMem::push(cbVirtualSharedTable* v) { m_dataFromDevice.push_back(v); }

void cbGraphSharedMem::push(cbMySQLCell* v) { m_dataPool.push_back(v); }

size_t cbGraphSharedMem::getMemUsed() {
  size_t tmp = sizeof(cbMySQLCell) * m_dataPool.size();
  for (auto& item : m_dataFromDevice) {
    auto __shape = item->getShape();
    tmp += sizeof(cbMySQLCell) * __shape[0] * __shape[1];
  }
  return tmp;
}

int32_t cbGraphSharedMem::getCellNum() {
  int32_t tmp = m_dataPool.size();
  for (auto& item : m_dataFromDevice) {
    auto __shape = item->getShape();
    tmp += __shape[0] * __shape[1];
  }
  return tmp;
}

cbNode::cbNode(const nodeType& nt) : nodeT(nt) {}

void cbNode::PointTo(cbNode* ptr) { nextNode = ptr; }

WFMySQLTask* cbNode::asSQLTask(void* metaTask) { return (WFMySQLTask*)metaTask; }

WFGoTask* cbNode::asGoTask(void* metaTask) { return (WFGoTask*)metaTask; }

cbVirtualDeviceNode::~cbVirtualDeviceNode() {
  // TODO
}

void* cbVirtualDeviceNode::generateTask() {
  // TODO
  return nullptr;
}

cbVirtualDeviceNode::cbVirtualDeviceNode() : cbNode(nodeType::Leaf) {}

cbOperatorNode::~cbOperatorNode() { delete Op; }

cbOperatorNode::cbOperatorNode(baseOp* op) : cbNode(nodeType::Operator), Op(op) {}

void* cbOperatorNode::generateTask() {
  // TODO
  return nullptr;
}

cbComputeGraph::cbComputeGraph(int32_t idx)
    : m_idx(idx),
      m_sharedMem(new cbGraphSharedMem()),
      m_sharedLuaStack(new cbGraphSharedLuaStack()),
      m_waitGroup(idx) {}

cbComputeGraph::~cbComputeGraph() {
  delete m_sharedMem;
  delete m_sharedLuaStack;
  for (auto item : m_nodes) { delete item; }
}

bool cbComputeGraph::isDAG() {
  // TODO
  return false;
}

bool cbComputeGraph::isSingleOutput() {
  // TODO
  return false;
}

cbMySQLCell* cbComputeGraph::createCell() {
  cbMySQLCell* ans = new cbMySQLCell();
  m_sharedMem->push(ans);
  return ans;
}

cbMySQLCell* cbComputeGraph::createCell(int value) {
  cbMySQLCell* ans = new cbMySQLCell(value);
  m_sharedMem->push(ans);
  return ans;
}

cbMySQLCell* cbComputeGraph::createCell(float value) {
  cbMySQLCell* ans = new cbMySQLCell(value);
  m_sharedMem->push(ans);
  return ans;
}

cbMySQLCell* cbComputeGraph::createCell(double value) {
  cbMySQLCell* ans = new cbMySQLCell(value);
  m_sharedMem->push(ans);
  return ans;
}

cbMySQLCell* cbComputeGraph::createCell(unsigned long long value) {
  cbMySQLCell* ans = new cbMySQLCell(value);
  m_sharedMem->push(ans);
  return ans;
}

cbMySQLCell* cbComputeGraph::createCell(const std::string& value) {
  cbMySQLCell* ans = new cbMySQLCell(value);
  m_sharedMem->push(ans);
  return ans;
}

cbMySQLCell* cbComputeGraph::createCell(const std::string& value, const cbMySQLType& t) {
  cbMySQLCell* ans = new cbMySQLCell(value, t);
  m_sharedMem->push(ans);
  return ans;
}

void cbComputeGraph::registerNode(cbNode* node) { m_nodes.push_back(node); }

int32_t cbComputeGraph::getId() { return m_idx; }

WFGraphTask* cbComputeGraph::generateGraphTask(const graph_callback& func) {
  WFGraphTask* graph = WFTaskFactory::create_graph_task([=](WFGraphTask* task) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "Graph task {} complete. Wakeup main process\n", m_idx);
    m_waitGroup.done();
  });
  if (!(isDAG() && isSingleOutput())) {
    fmt::print(fg(fmt::color::red), "The graph {} is not DAG or has multi output\n", m_idx);
    return nullptr;
  }
  // TODO pass all Tasks to graph.
  return graph;
}

void cbComputeGraph::execMain(WFGraphTask* task, cbComputeGraph* graph) {
  if (task != nullptr && graph != nullptr) {
    task->start();
    graph->m_waitGroup.wait();
  } else {
    fmt::print(fg(fmt::color::red), "The pointer to task or graph is nullptr\n");
  }
}

};  // namespace graph