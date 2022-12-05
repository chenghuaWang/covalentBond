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

namespace cb {
namespace trans {
void outbase(std::ofstream& oFile)  // 输入html框架
{
  oFile << "<html>";
  oFile << "<head>";
  oFile << "    <link type=\"text/css\" "
           "rel=\"stylesheet\"href=\"//repo.bfw.wiki/bfwrepo/css/litegraph.css\">";
  oFile << "    <script type=\"text/javascript\" "
           "src=\"//repo.bfw.wiki/bfwrepo/js/litegraph.js\"></script>";
  oFile << "</head>";
  oFile << "<body style='width:100%; height:100%'>";
  oFile
      << "    <canvas id='mycanvas' width='1024' height='720' style='border: 1px solid'></canvas>";
  oFile << "    <script>";
  oFile << "      var graph = new LGraph();\n";
  oFile << "      var canvas = new LGraphCanvas(\"#mycanvas\", graph);\n";
}

void outDeviceNode(std::ofstream& oFile, trivial::cbMySqlDevice* deviceNode, int node_num,
                   int device_num) {
  int posx = 100;
  int posy = 200 * device_num;
  std::string outtext, outtext2;
  outtext = "        function leafNode" + std::to_string(node_num) + "() {\n";
  oFile << outtext;
  outtext = deviceNode->host;
  outtext2 = "        this.addWidget(\"text\",\"host\",\"" + outtext + "\");\n";
  oFile << outtext2;
  outtext = deviceNode->port;
  outtext2 = "        this.addWidget(\"text\",\"port\",\"" + outtext + "\");\n";
  oFile << outtext2;
  outtext = deviceNode->usrName;
  outtext2 = "        this.addWidget(\"text\",\"userName\",\"" + outtext + "\");\n";
  oFile << outtext2;
  outtext = deviceNode->passWord;
  outtext2 = "        this.addWidget(\"text\",\"passWord\",\"" + outtext + "\");\n";
  oFile << outtext2;
  oFile << "        this.addOutput(\"data\", \"text\");\n";
  oFile << "        }\n";
  outtext = deviceNode->dataBaseName;
  outtext2 =
      "        leafNode" + std::to_string(node_num) + ".title = \"mysql_" + outtext + "\";\n";
  oFile << outtext2;
  outtext = "        LiteGraph.registerNodeType(\"basic/leafNode" + std::to_string(node_num)
            + "\", leafNode" + std::to_string(node_num) + ");\n";
  oFile << outtext;
  oFile << "        var node_leaf" + std::to_string(node_num)
               + " = LiteGraph.createNode(\"basic/leafNode" + std::to_string(node_num) + "\");\n";
  oFile << "        node_leaf" + std::to_string(node_num) + ".pos = [" + std::to_string(posx) + ","
               + std::to_string(posy) + "];\n";
  oFile << "        graph.add(node_leaf" + std::to_string(node_num) + ");\n";
}
void outOpNode(int node_num, int input_num, std::ofstream& oFile, int op_num) {
  int posx = 100 + 1 * 250;
  int posy = 350 + (op_num - 1) * 200;
  oFile << "        function opNode" + std::to_string(node_num) + "() {\n";
  for (int i = 1; i < input_num + 1; i++) {
    oFile << "        this.addInput(\"inputdata_" + std::to_string(i) + "\", \"text\");\n";
  }
  // oFile << "        this.addInput(\"inputdata_" + std::to_string(0) + "\", \"text\");\n";
  oFile << "        this.addOutput(\"combined_data\", \"text\");}\n";
  oFile << "        opNode" + std::to_string(node_num) + ".title = \"opNode\";\n";
  oFile << "        LiteGraph.registerNodeType(\"basic/opNode" + std::to_string(node_num)
               + "\", opNode" + std::to_string(node_num) + ");\n";
  oFile << "        var node_op" + std::to_string(node_num)
               + " = LiteGraph.createNode(\"basic/opNode" + std::to_string(node_num) + "\");\n";
  oFile << "        node_op" + std::to_string(node_num) + ".pos = [" + std::to_string(posx) + ","
               + std::to_string(posy) + "];\n";
  oFile << "        graph.add(node_op" + std::to_string(node_num) + ");\n";
}
void outFinNode(int device_num, int op_num, std::ofstream& oFile) {
  int posx = 100 + 2 * 250;
  int posy = 350 + 200 * (op_num / 2);
  oFile << "        function outputNode() {";
  for (int i = 0; i < op_num; i++) {
    oFile << "        this.addInput(\"outdata" + std::to_string(i) + "\", \"text\");}";
  }
  oFile << "        outputNode.title = \"outputNode\";";
  oFile << "        LiteGraph.registerNodeType(\"basic/outputNode\", outputNode);\n";
  oFile << "        var node_out = LiteGraph.createNode(\"basic/outputNode\");";
  oFile << "        node_out.pos = [" + std::to_string(posx) + "," + std::to_string(posy) + "];\n";
  oFile << "        graph.add(node_out);\n";
}
void Node_leaf_connect(int nodeleftnum, int noderightnum, int nowinputsnum, std::ofstream& oFile) {
  oFile << "        node_leaf" + std::to_string(nodeleftnum) + ".connect(0,node_op"
               + std::to_string(noderightnum) + "," + std::to_string(nowinputsnum) + ");\n";
}
void Node_op_connect(int nodeleftnum, int nowinputsnum, std::ofstream& oFile) {
  oFile << "        node_op" + std::to_string(nodeleftnum) + ".connect(0,node_out,"
               + std::to_string(nowinputsnum) + ");\n";
}

void outbaseo(std::ofstream& oFile) {
  oFile << "graph.start();\n";
  oFile << "    </script>\n";
  oFile << "</body>\n";
  oFile << "</html>\n";
}

}  // namespace trans
}  // namespace cb
