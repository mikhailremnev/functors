#include <string>
#include <iostream>
#include "functor.h"

namespace functor
{
  using namespace std;
  FUNCTOR_FROM_CONSTRUCTOR(string, const char*);
  FUNCTOR_FROM_METHOD(string, string, substr, int, int);
}

int main()
{
  Typeless s = functor::string_constructor("abcdefgh");

  std::cout << functor::string_substr(s, 1, 3) << std::endl;

  return 0;
}

