#include "api.hpp"

namespace cb {
namespace apiCPP {

cb::graph::cbComputeGraph* addGraph(int32_t idx, const std::string& cmd,
                                    trivial::cbVirtualDeviceManager* vdm) {
  cb::graph::cbComputeGraph* cbg = new cb::graph::cbComputeGraph(idx);
  cbg->setVirtualDeviceManager(vdm);
  cbg->execScript(cmd);
  return cbg;
}

}  // namespace apiCPP
}  // namespace cb