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

void graphContainer::execMain() {
  WFGraphTask* graph = WFTaskFactory::create_graph_task([=](WFGraphTask* task) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
               "Graph task complete. Wakeup main process\n");
  });
  WFTimerTask* timer = WFTaskFactory::create_timer_task(m_loopTime, 0, [=](WFTimerTask* task) {
    fmt::print("Loops Graphs by {} sec.", m_loopTime);
    this->execMain();
  });
  auto timerNode = &graph->create_graph_node(timer);
  for (auto& item : m_graphs) {
    (graph->create_graph_node(item->generateGraphTask()))-- > (*timerNode);
  }
  graph->start();
}

app::app(const appCfg& cfg)
    : m_graphs(cfg.graphExecSec), m_web(cfg.webPort, cfg.webRoot), m_rHttp(cfg.rHttpPort) {}

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
      return;
    }
    Json& kv = req->json();
    auto t =
        cb::apiCPP::addGraph(kv["id"].get<int>(), kv["script"].get<std::string>(), &this->m_VDM);
    this->m_graphs.addGraph(t);
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
}

}  // namespace pipeline
}  // namespace cb