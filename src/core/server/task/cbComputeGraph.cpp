#include "cbComputeGraph.hpp"
#include "trivial/cbVirtualDevice.hpp"

namespace cb {
namespace graph {

cbGraphSharedMem::~cbGraphSharedMem() {
  for (auto item : m_dataFromDevice) { delete item; }
  for (auto item : m_dataPool) { delete item; }
}

void cbGraphSharedMem::push(cbVirtualSharedTable* v) { m_dataFromDevice.push_back(v); }

void cbGraphSharedMem::push(cbMySQLCell* v) { m_dataPool.push_back(v); }

void cbGraphSharedMem::push(cbMySQLField* v) { m_fields.push_back(v); }

void cbGraphSharedMem::setOutStruct(const cbShape<2>& shape, cbMySQLField** info) {
  if (!m_outStruct) {
    m_outStruct = new cbOutputTableStruct(shape, info);
  } else {
    m_outStruct->update(shape, info);
  }
}

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

cbOutputTableStruct* cbGraphSharedMem::getOutStruct() { return m_outStruct; }

void cbGraphSharedMem::clear() {
  for (auto item : m_dataFromDevice) { delete item; }
  for (auto item : m_dataPool) { delete item; }
  for (auto item : m_fields) { delete item; }
  m_dataFromDevice.clear();
  m_dataPool.clear();
  m_fields.clear();
  // m_outStruct->clear();
}

void cbGraphSharedLuaStack::execScriptFile(const std::string& filePath) {
  m_lua().script_file(filePath);
}

void cbGraphSharedLuaStack::execScript(const std::string& script) { m_lua().script(script); }

luaJitThread& cbGraphSharedLuaStack::get() { return m_lua; }

cbNode::cbNode(const nodeType& nt) : nodeT(nt) {}

void cbNode::PointTo(cbNode* ptr) { nextNode = ptr; }

WFMySQLTask* cbNode::asSQLTask(void* metaTask) { return (WFMySQLTask*)metaTask; }

WFGoTask* cbNode::asGoTask(void* metaTask) { return (WFGoTask*)metaTask; }

cbVirtualDeviceNode::~cbVirtualDeviceNode() {
  // no members need to been free
}

void* cbVirtualDeviceNode::generateTask() {
  if (m_queries.size() != 1) {
    fmt::print(fg(fmt::color::red),
               "We have not support multi SQL sentence for now\n It's just for testing purpose\n");
    return nullptr;
  }
  return (void*)m_device->query(m_queries[0], [=](WFMySQLTask* task) -> void {
    // 1. Judge the graph is bind to this node.
    if (graph == nullptr) {
      fmt::print(fg(fmt::color::red), "Graph is not setted for this node.\n");
    }
    // 2. If the task is success.
    if (task->get_state() != WFT_STATE_SUCCESS) {
      fprintf(stderr, "task error=%d\n", task->get_error());
      return;
    }
    // 3. get the cursor.
    protocol::MySQLResultCursor cursor(task->get_resp());

    // 4. Judge the packet is ok.
    if (task->get_resp()->get_packet_type() == MYSQL_PACKET_ERROR) {
      fprintf(stderr, "ERROR. error_code=%d %s\n", task->get_resp()->get_error_code(),
              task->get_resp()->get_error_msg().c_str());
    }

    // store all data.
    cbVirtualSharedTable* __data = new cbVirtualSharedTable(&cursor);
    graph->m_sharedMem->push(__data);

    // Store the virtual table to the io.O port. And pass to the next Node's inputs port.
    mapShared2Virtual(__data, &io.O);
    if (nextNode) { nextNode->io.I.push_back(io.O); }
    if (isFinalOutput) {
      graph->m_sharedMem->setOutStruct(io.O.getShape(), io.O.getInfo());
      graph->io.O = io.O;
    }
    return;
  });
}

void cbVirtualDeviceNode::addQuery(const std::string& q) { m_queries.push_back(q); }

trivial::cbMySqlDevice* cbVirtualDeviceNode::getDevice() { return m_device; }

void cbVirtualDeviceNode::setMySQLDevice(trivial::cbMySqlDevice* device) { m_device = device; }

cbVirtualDeviceNode::cbVirtualDeviceNode() : cbNode(nodeType::Leaf) {}

cbRedisCachingNode::cbRedisCachingNode(int32_t idx) : cbNode(nodeType::Output), m_idx(idx) {}

cbRedisCachingNode::~cbRedisCachingNode() {}

void* cbRedisCachingNode::generateTask() {
  // TODO series set.
  return nullptr;
}

WFRedisTask* cbRedisCachingNode::_generateSetTask(const std::vector<std::string>& params,
                                                  const redis_callback& callback_func,
                                                  void* usrData, int32_t retryTimes) {
  return m_device->set(params, callback_func, usrData, retryTimes);
}

void cbRedisCachingNode::setRedisDevice(trivial::cbRedisDevice* device) { m_device = device; }

cbOperatorNode::~cbOperatorNode() { delete Op; }

cbOperatorNode::cbOperatorNode(baseOp* op) : cbNode(nodeType::Operator), Op(op) {}

void* cbOperatorNode::generateTask() {
  WFGoTask* goTask = WFTaskFactory::create_go_task("execMain", [=]() {
    Op->io.I = io.I;
    Op->execMain();
  });

  goTask->set_callback([=](WFGoTask* task) {
    if (task->get_state() == WFT_STATE_SUCCESS) {
      io.O = Op->io.O;
      if (isFinalOutput) {
        graph->m_sharedMem->setOutStruct(io.O.getShape(), io.O.getInfo());
        graph->io.O = io.O;
      }
      if (nextNode) { nextNode->io.I.push_back(io.O); }
    } else {
      fmt::print(fg(fmt::color::red), "Go Task exec failed.");
      return;
    }
    Op->io.I.clear();
  });

  return (void*)goTask;
}

void cbOperatorNode::overload(sol::function& funcPtr) { Op->overload(funcPtr); }

cbComputeGraph::cbComputeGraph(int32_t idx)
    : m_idx(idx),
      m_sharedMem(new cbGraphSharedMem()),
      m_sharedLuaStack(new cbGraphSharedLuaStack()) {
  // bind all functions to lua state.
  auto covalentBound = m_sharedLuaStack->get()()["Cb"].get_or_create<sol::table>();
  auto covalentBoundF = covalentBound["F"].get_or_create<sol::table>();
  auto covalentBoundTable = covalentBound["Data"].get_or_create<sol::table>();

  // bind numerous functions.
  covalentBoundF.set_function(
      "refNode", sol::overload([](cbVirtualDeviceNode* v) -> cbNode* { return (cbNode*)v; },
                               [](cbOperatorNode* v) -> cbNode* { return (cbNode*)v; }));

  covalentBoundF.set_function("__cpp_packedCellAsVec", &__luaPackedCellAsVec);
  covalentBoundF.set_function("__cpp_packedStringAsVec", &__luaPackedStringAsVec);

  // bind virtual table/shape, etc;
  covalentBoundTable.new_usertype<cbShape<2>>(
      "Shape",

      "numElements", &cbShape<2>::numElements,

      sol::meta_function::index, sol::resolve<int32_t&(cbShape<2>&, int32_t)>(fetchShapeIndex),

      sol::meta_function::new_index,
      sol::resolve<void(cbShape<2>&, int32_t, int32_t const&)>(storeShapeIndex)

  );

  covalentBoundTable.new_usertype<cbVirtualTable>(
      "KVTable",

      sol::meta_function::construct,
      sol::factories([](cbShape<2>& shape) { return MAKE_SHARED(cbVirtualTable)(shape); }),

      sol::call_constructor,
      sol::factories([](cbShape<2>& shape) { return MAKE_SHARED(cbVirtualTable)(shape); }),

      "resetShape", &cbVirtualTable::resetShape,

      "resetShapeH", &cbVirtualTable::resetShapeH,

      "setInfoAt", &cbVirtualTable::setInfoAt,

      "getInfoAt", &cbVirtualTable::getInfoAt,

      "setPtrAt", &cbVirtualTable::setPtrAt,

      "getShape", &cbVirtualTable::getShape,

      "atPtr", &cbVirtualTable::atPtr,

      "atPtrRef", &cbVirtualTable::atPtrRef,

      "colNameAt", &cbVirtualTable::colNameAt,

      "colTypeAt", &cbVirtualTable::colTypeAt,

      "pushRow", &cbVirtualTable::pushRow,

      "keyBy", &cbVirtualTable::keyBy,

      "getCol", &cbVirtualTable::getCol,

      "getRow", &cbVirtualTable::getRow,

      "print", &cbVirtualTable::str

  );

  covalentBound.new_usertype<cbMySQLField>(

      "KVField",

      "setTableName", &cbMySQLField::setTable

  );

  // bind enumerate cbMySQLType
  covalentBound.new_enum<cbMySQLType>(

      "CellType", {{"Float", cbMySQLType::Float},
                   {"Double", cbMySQLType::Double},
                   {"Int", cbMySQLType::Int},
                   {"ULL", cbMySQLType::ULL},
                   {"String", cbMySQLType::String},
                   {"Date", cbMySQLType::Date},
                   {"Time", cbMySQLType::Time},
                   {"DataTime", cbMySQLType::DataTime},
                   {"Null", cbMySQLType::Null}}

  );

  // bind graph.
  covalentBound.new_usertype<cbComputeGraph>(
      "ComputeGraph",

      sol::meta_function::construct,
      sol::factories([](const int32_t idx) { return MAKE_SHARED(cbComputeGraph)(idx); }),

      sol::call_constructor,
      sol::factories([](const int32_t idx) { return MAKE_SHARED(cbComputeGraph)(idx); }),

      "isDAG", &cbComputeGraph::isDAG,

      "isSingleOutput", &cbComputeGraph::isSingleOutput,

      "createKVCell",
      sol::overload([](cbComputeGraph& p) { return p.createCell(); },
                    [](cbComputeGraph& p, int value) { return p.createCell(value); },
                    [](cbComputeGraph& p, float value) { return p.createCell(value); },
                    [](cbComputeGraph& p, double value) { return p.createCell(value); },
                    [](cbComputeGraph& p, unsigned long long value) { return p.createCell(value); },
                    [](cbComputeGraph& p, const std::string& value) { return p.createCell(value); },
                    [](cbComputeGraph& p, const std::string& value, const cbMySQLType& t) {
                      return p.createCell(value, t);
                    }),

      "createVirtualDeviceNode", &cbComputeGraph::createVirtualDeviceNode,

      "createCombineNode", &cbComputeGraph::createCombineNode,

      "createRedisCachingNode", &cbComputeGraph::createRedisCachingNode,

      "addCacheServer", &cbComputeGraph::addCacheServer

  );

  // bind this to graph
  m_sharedLuaStack->get()()["ThisGraph"].get_or_create<cbComputeGraph>(this);

  // cbIO
  covalentBound.new_usertype<cbOpIO>(

      "OpIO",

      "I", &cbOpIO::I,

      "O", &cbOpIO::O

  );

  // bind base Op
  covalentBound.new_usertype<baseOp>(

      "cbBaseOp",

      "io", &baseOp::io

  );

  // bind CombineOp
  covalentBound.new_usertype<cbOpCombine>(

      "cbOpCombine",

      "overrideFunc", &cbOpCombine::overload

  );

  // bind cbVirtualDeviceNode
  covalentBound.new_usertype<cbVirtualDeviceNode>(

      "VirtualDeviceNode",

      "PointTo", &cbVirtualDeviceNode::PointTo,

      "addQuery", &cbVirtualDeviceNode::addQuery,

      "io", &cbVirtualDeviceNode::io

  );

  // bind caching node
  covalentBound.new_usertype<cbRedisCachingNode>("RedisCachingNode");

  // bind operation node
  covalentBound.new_usertype<cbOperatorNode>(

      "OperatorNode",

      "PointTo", &cbOperatorNode::PointTo,

      "io", &cbOperatorNode::io,

      "Op", &cbOperatorNode::Op

  );
}

cbComputeGraph::~cbComputeGraph() {
  delete m_sharedMem;
  delete m_sharedLuaStack;
  for (auto item : m_nodes) { delete item; }
}

bool cbComputeGraph::isDAG() {
  if (!isSingleOutput()) return false;
  // n^2 is ok, the graph is not such big.
  for (auto item : m_nodes) {
    cbNode* cur = item;
    cbNode* next = cur;
    while (next->nextNode != nullptr) {
      next = next->nextNode;
      if (next == cur) { return false; }
    }
  }
  return true;
}

bool cbComputeGraph::isSingleOutput() {
  int __singleOutCnt = 0;
  for (auto& item : m_nodes) {
    if (item->nextNode == nullptr) {
      __singleOutCnt++;
      item->isFinalOutput = true;
    }
  }
  return __singleOutCnt == 1 ? true : false;
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

cbVirtualDeviceNode* cbComputeGraph::createVirtualDeviceNode(int32_t idx) {
  cbVirtualDeviceNode* ans = new cbVirtualDeviceNode();
  ans->setMySQLDevice(m_virtualDevice->getMySqlDevice(idx));
  this->registerNode(ans);
  return ans;
}

cbRedisCachingNode* cbComputeGraph::createRedisCachingNode(int32_t idx) {
  cbRedisCachingNode* ans = new cbRedisCachingNode(idx);
  ans->setRedisDevice(m_virtualDevice->getRedisDevice(idx));
  return ans;
}

cbOperatorNode* cbComputeGraph::createCombineNode(const std::vector<std::string>& keys,
                                                  const std::string& name) {
  cbOpCombine* ansOp = new cbOpCombine(keys, name);

  ansOp->overload(this->m_sharedLuaStack->get()()["Cb"]["Op"]["CombineOp"]);

  cbOperatorNode* ans = new cbOperatorNode(ansOp);
  this->registerNode(ans);
  return ans;
}

void cbComputeGraph::setVirtualDeviceManager(trivial::cbVirtualDeviceManager* virtualDeviceM) {
  m_virtualDevice = virtualDeviceM;
}

void cbComputeGraph::registerNode(cbNode* node) {
  m_nodes.push_back(node);
  node->graph = this;
}

int32_t cbComputeGraph::getId() { return m_idx; }

WFGraphTask* cbComputeGraph::generateGraphTask(const graph_callback& func) {
  WFGraphTask* graph = WFTaskFactory::create_graph_task([=](WFGraphTask* task) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "Graph task {} complete. Wakeup main process\n", m_idx);
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "--------------------------------------------------\n");

    // If the caching set is not nullptr. Caching all table to l3 redis server.
    if (m_cacheNode && m_sharedMem->getOutStruct() != nullptr) {
      int32_t row = m_sharedMem->getOutStruct()->m_shape[0];
      int32_t col = m_sharedMem->getOutStruct()->m_shape[1];
      auto gos = m_sharedMem->getOutStruct();
      for (int32_t i = 0; i < row; ++i) {
        for (int32_t j = 0; j < col; ++j) {
          // To String and get packed to key and value.
          std::vector<std::string> kv;
          kv.push_back(gos->genKey4Redis(i, j));
          auto cell = io.O.atPtr(i, j);
          if (cell->isDate() || cell->isDatetime() || cell->isString()) {
            kv.push_back(cell->asString());
          } else if (cell->isInt()) {
            kv.push_back(std::to_string(cell->asInt()));
          } else if (cell->isFloat()) {
            kv.push_back(std::to_string(cell->asFloat()));
          } else if (cell->isULL()) {
            kv.push_back(std::to_string(cell->asULL()));
          } else if (cell->asDouble()) {
            kv.push_back(std::to_string(cell->asDouble()));
          }

          // Generate redis tasks.
          auto redisSetTask = m_cacheNode->_generateSetTask(
              kv, [](WFRedisTask* task) {}, nullptr, 3);
          redisSetTask->start();
        }
      }
    }
    // clear tmp memory;
    m_sharedMem->clear();
    for (auto& item : m_nodes) { item->io.I.clear(); }
  });
  if (!(isDAG() && isSingleOutput())) {
    fmt::print(fg(fmt::color::red), "The graph {} is not DAG or has multi outputs\n", m_idx);
    return nullptr;
  }
  std::map<cbNode*, WFGraphNode*> __cb2WF;

  WFGraphNode* __tail = nullptr;

  for (auto item : m_nodes) {
    switch (item->nodeT) {
      case nodeType::Leaf:
        __cb2WF[item] = &graph->create_graph_node((WFMySQLTask*)item->generateTask());
        break;
      case nodeType::Operator:
        __cb2WF[item] = &graph->create_graph_node((WFGoTask*)item->generateTask());
        break;
      case nodeType::Output: break;
    }
  }
  for (auto item : m_nodes) {
    if (item->nextNode != nullptr) [[likely]] {
      (*__cb2WF[item])-- > (*__cb2WF[item->nextNode]);
    } else {
      __tail = __cb2WF[item];
    }
  }
  return graph;
}

void cbComputeGraph::execMain(WFGraphTask* task, cbComputeGraph* graph) {
  if (task != nullptr && graph != nullptr) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "--------------------------------------------------\n");
    task->start();
  } else {
    fmt::print(fg(fmt::color::red), "The pointer to task or graph is nullptr\n");
  }
}

void cbComputeGraph::execScriptFile(const std::string& filePath) {
  m_sharedLuaStack->execScriptFile(filePath);
}

void cbComputeGraph::execScript(const std::string& script) { m_sharedLuaStack->execScript(script); }

void cbComputeGraph::addCacheServer(cbRedisCachingNode* v) {
  m_cacheNode = v;
  v->graph = this;
}

cbOutputTableStruct* cbComputeGraph::getOutput() { return m_sharedMem->getOutStruct(); }

std::vector<cbNode*> cbComputeGraph::getNodes() { return m_nodes; }

};  // namespace graph
}  // namespace cb