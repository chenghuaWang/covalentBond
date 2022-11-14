#include "task/cbComputeGraph.hpp"
#include <workflow/WFGraphTask.h>

using namespace graph;
using namespace trivial;

int main() {
  cbComputeGraph* cbg = new cbComputeGraph(0);
  cbVirtualDeviceManager* cbVDM = new cbVirtualDeviceManager();

  cbVDM->addMySqlDevice(new cbMySqlDevice(cbVirtualDeviceManager::m_numsMySql, "3306",
                                          "27.208.84.114", "django_user", "django_user", "testdb"));

  cbg->setVirtualDeviceManager(cbVDM);
  auto vdn_1 = cbg->createVirtualDeviceNode(0);
  auto vdn_2 = cbg->createVirtualDeviceNode(0);

  vdn_1->PointTo(vdn_2);

  vdn_1->addQuery("SELECT * FROM runoob_tbl;");
  vdn_2->addQuery("SELECT * FROM runoob_tbl;");

  cbComputeGraph::execMain(cbg->generateGraphTask(), cbg);

  delete cbg;
  delete cbVDM;
}