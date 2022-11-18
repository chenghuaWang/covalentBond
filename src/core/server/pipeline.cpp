#include "pipeline.hpp"

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

}  // namespace pipeline
}  // namespace cb