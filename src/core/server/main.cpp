#include "pipeline.hpp"

static WFFacilities::WaitGroup wait_group(3);

void sig_handler(int signo) {
  fmt::print("Bye.\n");
  wait_group.done();
  wait_group.done();
  wait_group.done();
  exit(0);
}

int main() {
  // cbComputeGraph* cbg = new cbComputeGraph(0);
  // cbVirtualDeviceManager* cbVDM = new cbVirtualDeviceManager();

  // // Connection 0
  // cbVDM->addMySqlDevice(new cbMySqlDevice(cbVirtualDeviceManager::m_numsMySql, "3306",
  //                                         "27.208.84.114", "django_user", "django_user",
  //                                         "testdb"));

  // // Connection 1
  // cbVDM->addMySqlDevice(new cbMySqlDevice(cbVirtualDeviceManager::m_numsMySql, "3306",
  //                                         "27.208.84.114", "django_user", "django_user",
  //                                         "testdb"));

  // cbg->setVirtualDeviceManager(cbVDM);

  // cbg->execScript(
  //     "node_vd_1 = ThisGraph:createVirtualDeviceNode(0);"
  //     "node_vd_2 = ThisGraph:createVirtualDeviceNode(1);"
  //     "node_vd_1:addQuery(\"SELECT * FROM runoob_tbl;\");"
  //     "node_vd_2:addQuery(\"SELECT * FROM runoob_tbl;\");"
  //     "node_combine_op = ThisGraph:createCombineNode(Cb.F.PackedStringToVec(\"a\", \"b\"));"
  //     "node_vd_1:PointTo(Cb.F.refNode(node_combine_op));"
  //     "node_vd_2:PointTo(Cb.F.refNode(node_combine_op));");

  // cbComputeGraph::execMain(cbg->generateGraphTask(), cbg);

  // delete cbg;
  // delete cbVDM;
  signal(SIGINT, sig_handler);
  char root[] = "/home/wang/covalentBond/bin/";
  auto App = cb::pipeline::app(cb::pipeline::appCfg{
      .webPort = 8888,
      .webRoot = root,
      .rHttpPort = 8080,
      .graphExecSec = 5,
  });
  App.execMain();
  wait_group.wait();
  App.stopMain();
}