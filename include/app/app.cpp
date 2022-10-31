#include <string>
#include <utility>
#include <vector>

#include "utils/cmdline.hpp"

struct CmdlineArgs {
  bool male;
  int id;
  double height;
  std::string name;
  std::vector<std::string> grade;
};

inline auto Init(int argc, char* argv[]) -> void {
  ascpp::Option<int> o{'o', "opt", "option"};
  ascpp::Init("mrbeardad", "ascpp", "awesome cpp framework", "0.1.0");
  ascpp::ParseCmdlineOpt(argc, argv, Option<bool>{'b', "bool", "boolean"},
                         Option<int>{'i', "int", "integer"}, Option<int>{'i', "int", "integer"},
                         Option<int>{'i', "int", "integer"}, Option<int>{'i', "int", "integer"},
                         Option<int>{'i', "int", "integer"});
  ascpp::GetCmdlineArg
}
