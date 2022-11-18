#ifndef __SERVER_PIPELINE_HPP_
#define __SERVER_PIPELINE_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include <workflow/WFGraphTask.h>
#include <workflow/WFTaskFactory.h>

#include "cbWebserver.hpp"
#include "task/cbComputeGraph.hpp"

namespace cb {
namespace pipeline {

class graphContainer {
 public:
  ~graphContainer();
  graphContainer(int32_t n);
  graphContainer(graphContainer&) = delete;
  graphContainer(const graphContainer&) = delete;
  graphContainer operator=(graphContainer&) = delete;
  graphContainer operator=(const graphContainer&) = delete;

  void addGraph(graph::cbComputeGraph* g);
  void execMain();

 private:
  std::vector<graph::cbComputeGraph*> m_graphs;
  int32_t m_loopTime = 10;  ///! nano seconds.
};

class app {
 public:
 private:
};

}  // namespace pipeline
}  // namespace cb

#endif