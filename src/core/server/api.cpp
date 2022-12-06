#include "api.hpp"
using namespace wfrest;
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
void outbase(HttpResp* resp)  // 输入html框架
{
  resp->String("<html>");
  resp->String("<head>");
  resp->String(
      "    <link "
      "type=\"text/css\"rel=\"stylesheet\"href=\"//repo.bfw.wiki/bfwrepo/css/litegraph.css\">");
  resp->String("    <script "
               "type=\"text/javascript\"src=\"//repo.bfw.wiki/bfwrepo/js/litegraph.js\"></script>");
  resp->String("</head>");
  resp->String("<body style='width:100%; height:100%'>");
  resp->String(
      "    <canvas id='mycanvas' width='1024' height='720' style='border: 1px solid'></canvas>\n");
  resp->String("    <script>\n");
  resp->String("      var graph = new LGraph();\n");
  resp->String("      var canvas = new LGraphCanvas(\"#mycanvas\", graph);\n");
}

void outDeviceNode(HttpResp* resp, trivial::cbMySqlDevice* deviceNode, int node_num,
                   int device_num) {
  int posx = 100;
  int posy = 200 * device_num;
  std::string outtext, outtext2;
  outtext = "        function leafNode" + std::to_string(node_num) + "() {\n";
  resp->String(outtext);
  outtext = deviceNode->host;
  outtext2 = "        this.addWidget(\"text\",\"host\",\"" + outtext + "\");\n";
  resp->String(outtext2);
  outtext = deviceNode->port;
  outtext2 = "        this.addWidget(\"text\",\"port\",\"" + outtext + "\");\n";
  resp->String(outtext2);
  outtext = deviceNode->usrName;
  outtext2 = "        this.addWidget(\"text\",\"userName\",\"" + outtext + "\");\n";
  resp->String(outtext2);
  outtext = deviceNode->passWord;
  outtext2 = "        this.addWidget(\"text\",\"passWord\",\"" + outtext + "\");\n";
  resp->String(outtext2);
  resp->String("        this.addOutput(\"data\", \"text\");\n");
  resp->String("        }\n");
  outtext = deviceNode->dataBaseName;
  outtext2 =
      "        leafNode" + std::to_string(node_num) + ".title = \"mysql_" + outtext + "\";\n";
  resp->String(outtext2);
  outtext = "        LiteGraph.registerNodeType(\"basic/leafNode" + std::to_string(node_num)
            + "\", leafNode" + std::to_string(node_num) + ");\n";
  resp->String(outtext);
  resp->String("        var node_leaf" + std::to_string(node_num)
               + " = LiteGraph.createNode(\"basic/leafNode" + std::to_string(node_num) + "\");\n");

  outtext = "        node_leaf" + std::to_string(node_num) + ".pos = [" + std::to_string(posx) + ","
            + std::to_string(posy) + "];\n";
  resp->String(outtext);
  outtext = "        graph.add(node_leaf" + std::to_string(node_num) + ");\n";
  resp->String(outtext);
}
void outOpNode(int node_num, int input_num, HttpResp* resp, int op_num) {
  int posx = 100 + 1 * 250;
  int posy = 350 + (op_num - 1) * 200;
  std::string outtext;
  outtext = "        function opNode" + std::to_string(node_num) + "() {\n";
  resp->String(outtext);
  for (int i = 1; i < input_num + 1; i++) {
    outtext = "        this.addInput(\"inputdata_" + std::to_string(i) + "\", \"text\");\n";
    resp->String(outtext);
  }
  // oFile << "        this.addInput(\"inputdata_" + std::to_string(0) + "\", \"text\");\n";
  outtext = "        this.addOutput(\"combined_data\", \"text\");}\n";
  resp->String(outtext);
  outtext = "        opNode" + std::to_string(node_num) + ".title = \"opNode\";\n";
  resp->String(outtext);
  outtext = "        LiteGraph.registerNodeType(\"basic/opNode" + std::to_string(node_num)
            + "\", opNode" + std::to_string(node_num) + ");\n";
  resp->String(outtext);
  outtext = "        var node_op" + std::to_string(node_num)
            + " = LiteGraph.createNode(\"basic/opNode" + std::to_string(node_num) + "\");\n";
  resp->String(outtext);
  outtext = "        node_op" + std::to_string(node_num) + ".pos = [" + std::to_string(posx) + ","
            + std::to_string(posy) + "];\n";
  resp->String(outtext);
  outtext = "        graph.add(node_op" + std::to_string(node_num) + ");\n";
  resp->String(outtext);
}
void outFinNode(int device_num, int op_num, HttpResp* resp) {
  int posx = 100 + 2 * 250;
  int posy = 350 + 200 * (op_num / 2);
  std::string outtext;
  outtext = "        function outputNode() {";
  resp->String(outtext);
  for (int i = 0; i < op_num; i++) {
    outtext = "        this.addInput(\"outdata" + std::to_string(i) + "\", \"text\");}";
    resp->String(outtext);
  }

  outtext = "        outputNode.title = \"outputNode\";";
  resp->String(outtext);
  outtext = "        LiteGraph.registerNodeType(\"basic/outputNode\", outputNode);\n";
  resp->String(outtext);
  outtext = "        var node_out = LiteGraph.createNode(\"basic/outputNode\");";
  resp->String(outtext);
  outtext = "        node_out.pos = [" + std::to_string(posx) + "," + std::to_string(posy) + "];\n";
  resp->String(outtext);
  outtext = "        graph.add(node_out);\n";
  resp->String(outtext);
}
void Node_leaf_connect(int nodeleftnum, int noderightnum, int nowinputsnum, HttpResp* resp) {
  std::string outtext;
  outtext = "        node_leaf" + std::to_string(nodeleftnum) + ".connect(0,node_op"
            + std::to_string(noderightnum) + "," + std::to_string(nowinputsnum) + ");\n";
  resp->String(outtext);
}
void Node_op_connect(int nodeleftnum, int nowinputsnum, HttpResp* resp) {
  std::string outtext;
  outtext = "        node_op" + std::to_string(nodeleftnum) + ".connect(0,node_out,"
            + std::to_string(nowinputsnum) + ");\n";
  resp->String(outtext);
}

void outbaseo(HttpResp* resp) {
  std::string outtext;
  outtext = "graph.start();\n";
  resp->String(outtext);
  outtext = "    </script>\n";
  resp->String(outtext);
  outtext = "</body>\n";
  resp->String(outtext);
  outtext = "</html>\n";
  resp->String(outtext);
}

}  // namespace trans
}  // namespace cb
