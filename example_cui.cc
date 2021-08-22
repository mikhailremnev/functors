#include "functor.h"
#include <sstream>
#include <iostream>

FUNCTOR(help, int /*ignored*/)
{
  std::string ret = "";
  ret += "List of supported functions:\n";
  std::map<std::string, Functor*>::iterator it;
  for (it = func_map.begin(); it != func_map.end(); ++it) {
    ret += "  " + it->first + "(" + it->second->getArgs() + ")\n";
  }

  return ret;
}

FUNCTOR(sum, int x, int y)
{
  printf("%d + %d = %d\n", x, y, x+y);
  return std::string("The result is ") + std::to_string(x+y);
}

std::string parse(std::string cmd)
{
  std::stringstream words_stream(cmd);
  std::string word;
  std::vector<std::string> words;

  std::string cmd_name = "";

  //== Split input by whitespaces.

  while (words_stream >> word) {
    if (cmd_name == "") {
      cmd_name = word;
    } else {
      words.push_back(word);
    }
  }

  std::string ret = "";

  if (func_map.find(cmd_name) == func_map.end()) {
    ret += "Function '" + cmd_name + "' not found. Type 'help' for the list of supported functions.\n";
  } else {
    try {
      std::string result = func_map[cmd_name]->call(words);
      ret += result;
    } catch (std::invalid_argument &e) {
      ret = e.what();
    }
  }
  return ret;
}

int main()
{
  //== Start shell
  std::string line;

  printf("Welcome to the shell. Run 'help' to get the list of available commands.\n");
  printf("Usage example: 'sum 3 5'. Last return line is a netstring (str_len:str_content)\n\n");

  // TODO: Move from zero-terminated command outputs to netstrings?
  while (true) {
    std::cout << "\n> ";
    std::getline(std::cin, line);
    if (line == "exit") break;
    if (line.size() < 1) break;

    std::string ret = parse(line);

    //=== For netstrings
    std::cout << ret.size() << ":" << ret;
    //===

    //=== For zero-terminated strings.
    // std::cout << ret;
    // putchar(0);
    //===
  }
}

