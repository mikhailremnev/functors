/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2022 Mikhail Remnev
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include "functor.h"
#include <sstream>
#include <iostream>

FUNCTOR(help)
{
  std::string ret = "";
  ret += "List of supported functions:\n";
  std::map<std::string, Functor*>::iterator it;
  for (it = func_map().begin(); it != func_map().end(); ++it) {
    ret += "  " + it->first + "(" + it->second->getArgs() + ")\n";
  }

  return ret;
}

FUNCTOR(sum, int x, int y)
{
  printf("%d + %d = %d\n", x, y, x+y);
  return std::string("The result is ") + std::to_string(x+y);
}

FUNCTOR(prod, double x, double y)
{
  printf("%lf * %lf = %lf\n", x, y, x*y);
  return std::string("The result is ") + std::to_string(x*y);
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

  if (func_map().find(cmd_name) == func_map().end()) {
    ret += "Function '" + cmd_name + "' not found. Type 'help' for the list of supported functions.\n";
  } else {
    try {
      std::string result = func_map(cmd_name)->call(words);
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

