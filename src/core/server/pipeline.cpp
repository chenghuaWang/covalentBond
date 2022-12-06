#include <wfrest/json.hpp>
#include "pipeline.hpp"
#include "api.hpp"

using namespace wfrest;
using Json = nlohmann::json;

namespace cb {
namespace pipeline {

graphContainer::graphContainer(int32_t n) : m_loopTime(n) {}

graphContainer::~graphContainer() {
  for (auto item : m_graphs) { delete item; }
}

void graphContainer::addGraph(graph::cbComputeGraph* g) { m_graphs.push_back(g); }

graph::cbComputeGraph* graphContainer::getGraph(int32_t idx) {
  for (auto& item : m_graphs) {
    if (item->getId() == idx) { return item; }
  }
  return nullptr;
}

void graphContainer::execMain() {
  if (!m_isTerminated) {
    WFGraphTask* graph = WFTaskFactory::create_graph_task([=](WFGraphTask* task) {
      fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
                 "Graph task complete. Wakeup main process\n");
      fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
                 "**************************************************\n");
    });
    WFTimerTask* timer = WFTaskFactory::create_timer_task(m_loopTime, 0, [=](WFTimerTask* task) {
      fmt::print("Loops Graphs by {} sec.\n", m_loopTime);
      if (!m_isTerminated) {
        this->execMain();
      } else {
        fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
                   "Terminated. End loop the graph task. Waiting the graph in queue done.\n");
      }
    });
    auto timerNode = &graph->create_graph_node(timer);
    for (auto& item : m_graphs) {
      (graph->create_graph_node(item->generateGraphTask()))-- > (*timerNode);
    }
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "**************************************************\n");
    graph->start();
  } else {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "Terminated. End loop the graph task. Waiting the graph in queue done.\n");
  }
}

void graphContainer::setTerminated(bool enable) { m_isTerminated = enable; }

app::app(const appCfg& cfg)
    : m_graphs(cfg.graphExecSec), m_web(cfg.webPort, cfg.webRoot), m_rHttp(cfg.rHttpPort) {
  // connect Redis
  int32_t redisCache = trivial::cbVirtualDeviceManager::m_numsRedis;
  this->m_VDM.addRedisDevice(new trivial::cbRedisDevice(redisCache, cfg.redisPort, cfg.redisHost,
                                                        "", cfg.redisPassword, cfg.redisDBNum,
                                                        cfg.redisSSL));
  trivial::cbVirtualDeviceManager::m_numsRedis++;
}

void app::initRHttp() {
  /**
   * @brief add_graph. POST.
   * type: APPLICATION_JSON
   * id: the index of the graph.
   * script: the script that create the graph.
   *
   * @note curl -H "Content-Type: application/json" -X POST -d "{\"id\": 0, \"script\": \"I am \n
   * iron man\"}" "http://localhost:8080/add_graph"
   *
   */
  m_rHttp().POST("/add_graph", [this](const HttpReq* req, HttpResp* resp) {
    if (req->content_type() != APPLICATION_JSON) {
      resp->set_status(HttpStatusBadRequest);
      resp->set_header_pair("Content-Type", "application/json");
      resp->append_output_body("{\"res\": \"False\"}");
      return;
    }
    Json& kv = req->json();
    auto t =
        cb::apiCPP::addGraph(kv["id"].get<int>(), kv["script"].get<std::string>(), &this->m_VDM);
    if (!t) {
      resp->set_status(HttpStatusBadRequest);
      resp->set_header_pair("Content-Type", "application/json");
      resp->append_output_body("{\"res\": \"False\"}");
      return;
    }
    this->m_graphs.addGraph(t);
    resp->set_header_pair("Content-Type", "application/json");
    resp->append_output_body("{\"res\": \"True\"}");
  });

  m_rHttp().POST("/add_device", [this](const HttpReq* req, HttpResp* resp) {
    if (req->content_type() != APPLICATION_JSON) {
      resp->set_status(HttpStatusBadRequest);
      resp->set_header_pair("Content-Type", "application/json");
      resp->append_output_body("{\"res\": \"False\"}");
      return;
    }
    Json& kv = req->json();

    // get the device type.
    std::string deviceType = kv["deviceType"].get<std::string>();

    if (deviceType == "MySQL") {
      std::string host = kv["host"].get<std::string>();
      std::string port = kv["port"].get<std::string>();
      std::string usrName = kv["usrName"].get<std::string>();
      std::string password = kv["password"].get<std::string>();
      std::string databaseName = kv["databaseName"].get<std::string>();
      int32_t curIdx = trivial::cbVirtualDeviceManager::m_numsMySql;
      this->m_VDM.addMySqlDevice(
          new trivial::cbMySqlDevice(trivial::cbVirtualDeviceManager::m_numsMySql, port, host,
                                     usrName, password, databaseName));
      resp->set_header_pair("Content-Type", "application/json");
      resp->append_output_body(fmt::format("{}\"res\": \"{}\" {}", "{", curIdx, "}"));
    } else if (deviceType == "Redis") {
      std::string host = kv["host"].get<std::string>();
      std::string port = kv["port"].get<std::string>();
      std::string usrName = kv["usrName"].get<std::string>();
      std::string password = kv["password"].get<std::string>();
      // TODO
    }
  });

  m_rHttp().GET("/table_at_graph", [=](const HttpReq* req, HttpResp* resp) {
    if (!req->has_query("idx")) {
      resp->set_status(500);
      return;
    }
    int32_t _graphId = atoi(req->query("idx").c_str());
    if (m_graphs.getGraph(_graphId) == nullptr) { return; }
    auto outs = m_graphs.getGraph(_graphId)
                    ->getOutput();  //! first: the output structure. second: true data
    Json kv;
    kv["row"] = outs->m_shape[0];
    kv["col"] = outs->m_shape[1];
    if (outs->m_info.size() == 0) {
      kv["tableName"] = "DefaultName";
    } else {
      kv["tableName"] = outs->m_info[0].getTable();
    }
    std::vector<std::string> _tmpColName;
    for (auto& item : outs->m_info) { _tmpColName.push_back(item.getName()); }
    kv["colName"] = _tmpColName;
    resp->Json(kv);
  });
  m_rHttp().GET("/trans_graph", [=](const HttpReq* req, HttpResp* resp) {
    if (!req->has_query("idx")) {
      resp->set_status(500);
      return;
    }
    const std::string& content_type = req->header("html");
    int32_t _graphId = atoi(req->query("idx").c_str());
    if (m_graphs.getGraph(_graphId) == nullptr) { return; }
    auto outs = m_graphs.getGraph(_graphId);
    auto nodes = outs->getNodes();
    resp->String(content_type);
    trans::outbase(resp);
    std::map<cb::graph::cbNode*, cb::trans::opMapStruct> mapOpNode;
    std::map<cb::graph::cbNode*, cb::trans::leafMapStruct> mapLeafNode;
    std::map<int, cb::graph::cbNode*> toLeaf, toOpNode;
    int opCodeNow = 0;
    int leafCodeNow = 0;
    for (auto item : nodes) {
      switch (item->nodeT) {
        case cb::graph::nodeType::Leaf: {
          mapLeafNode[item].nodeCode = ++leafCodeNow;
          mapOpNode[item->nextNode].inputNodeCode.push_back(leafCodeNow);
          mapOpNode[item->nextNode].inputNum++;
          toLeaf[leafCodeNow] = item;
          break;
        }
        case cb::graph::nodeType::Operator: {
          opCodeNow++;
          mapOpNode[item].nodeCode = opCodeNow;
          toOpNode[opCodeNow] = item;
          break;
        }
        case cb::graph::nodeType::Output: break;
      }
    }
    // set the node of position(Y)
    int firstPosy = 0;
    for (int i = 1; i < opCodeNow + 1; i++) {
      firstPosy = firstPosy + 100;
      auto opnode = toOpNode[i];
      mapOpNode[opnode].posy = firstPosy;
      for (auto j : mapOpNode[opnode].inputNodeCode) {
        auto leafNode = toLeaf[j];
        mapLeafNode[leafNode].posy = firstPosy;
        firstPosy = firstPosy + 200;
      }
      mapOpNode[opnode].posy = (firstPosy + mapOpNode[opnode].posy) / 2;
    }

    int finNodePosy = (mapOpNode[toOpNode[1]].posy + mapOpNode[toOpNode[opCodeNow]].posy) / 2;
    trans::createFinNode(finNodePosy, opCodeNow, resp);
    for (int i = 1; i < opCodeNow + 1; i++) {
      auto opnode = toOpNode[i];
      trans::createOpNode(resp, mapOpNode[opnode]);
      for (auto j : mapOpNode[opnode].inputNodeCode) {
        auto leafNode = toLeaf[j];
        auto device_n = (graph::cbVirtualDeviceNode*)leafNode;
        auto deviceNode = device_n->getDevice();
        trans::createLeafNode(resp, mapLeafNode[leafNode], deviceNode);
        trans::Node_leaf_connect(mapLeafNode[leafNode].nodeCode, i, mapOpNode[opnode].inputNumNow,
                                 resp);
        mapOpNode[opnode].inputNumNow++;
      }
      trans::Node_op_connect(i, i - 1, resp);
    }
    cb::trans::outbaseo(resp);
  });
}

void app::execMain() {
  // init all
  initRHttp();

  // exec all
  m_graphs.execMain();
  m_rHttp.execMain();
  m_web.execMain();
}

void app::stopMain() {
  m_rHttp.stopMain();
  m_web.stopMain();
  m_graphs.setTerminated(true);
}

}  // namespace pipeline
}  // namespace cb