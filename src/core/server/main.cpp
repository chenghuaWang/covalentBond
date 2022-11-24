#include "argparse.hpp"
#include "pipeline.hpp"

#define HELP_STR \
  "\
Licensed under the Apache License, Version 2.0 (the \" License \")\n\
You may obtain a copy of the License at\n\
\n\
http://www.apache.org/licenses/LICENSE-2.0\n\
\n\
Unless required by applicable law or agreed to in writing, software\n\
distributed under the License is distributed on an \" AS IS \" BASIS,\n\
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n\
See the License for the specific language governing permissions and\n\
limitations under the License.\n"

#define WELCOME_STR "Welcome to CovalentBond."

static WFFacilities::WaitGroup wait_group(1);
static WFFacilities::WaitGroup wait_group_end(1);

void sig_handler(int signo) { wait_group.done(); }

int main(int argc, char* argv[]) {
  argp::parser args;
  args.add_arg("--help", "-H", false);
  args.add_arg<std::string>("--webRoot", "-R", argp::arg_type::t_string, ".", true);
  args.add_arg<int>("--webPort", "-Wp", argp::arg_type::t_int, 8888, true);
  args.add_arg<int>("--httpPort", "-Hp", argp::arg_type::t_int, 8080, true);
  args.add_arg<int>("--execSec", "-S", argp::arg_type::t_int, 10, false);

  args.parse(argc, argv);

  if (args.has_item("help")) {
    std::cout << HELP_STR << std::endl;
    args.show_all_defined();
    exit(0);
  }

  cb::pipeline::appCfg cfg;

  cfg.webPort = args.get_item<int>("webPort");
  cfg.webRoot = args.get_item<std::string>("webRoot").c_str();
  cfg.rHttpPort = args.get_item<int>("httpPort");
  cfg.graphExecSec = args.get_item<int>("execSec");

  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGKILL, sig_handler);

  std::cout << HELP_STR << std::endl;
  std::cout << WELCOME_STR << std::endl;

  auto App = cb::pipeline::app(cfg);
  App.execMain();
  wait_group.wait();
  App.stopMain();
  // Waiting for 10 sec for graph tasks in queue done.
  fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic,
             "Waiting about 10 sec for graph tasks in queue done.\n");
  WFTimerTask* timerToEnd = WFTaskFactory::create_timer_task(10, 0, [=](WFTimerTask* task) {
    fmt::print(fg(fmt::color::steel_blue) | fmt::emphasis::italic, "All tasks were done.\n");
    wait_group_end.done();
  });
  timerToEnd->start();
  wait_group_end.wait();
  fmt::print("\nBye.\n");
  return 0;
}
