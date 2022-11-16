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

  cbg->execScript("node_vd_1 = ThisGraph:createVirtualDeviceNode(0);"
                  "node_vd_2 = ThisGraph:createVirtualDeviceNode(0);"
                  "node_vd_1:addQuery(\"SELECT * FROM runoob_tbl;\");"
                  "node_vd_2:addQuery(\"SELECT * FROM runoob_tbl;\");"
                  "node_vd_1:PointTo(Cb.F.refNode(node_vd_2));"
                  "shape_test = Cb.F.makeShapeFull(10, 10);"
                  "shape_test[0] = 12;"
                  "print(shape_test[0]);"
                  "tmp = Cb.F.createKVCell(ThisGraph, 5);"
                  "print(tmp:asInt());");

  // auto vdn_1 = cbg->createVirtualDeviceNode(0);
  // auto vdn_2 = cbg->createVirtualDeviceNode(0);

  // vdn_1->PointTo(vdn_2);

  // vdn_1->addQuery("SELECT * FROM runoob_tbl;");
  // vdn_2->addQuery("SELECT * FROM runoob_tbl;");

  cbComputeGraph::execMain(cbg->generateGraphTask(), cbg);

  delete cbg;
  delete cbVDM;
}