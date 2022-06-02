#include "functor.h"

// Creating named function "f"
FUNCTOR(f, int i, const char* c)
{
  int j = 6;
  printf("i = %d, c = %s\n", i, c);

  return 5;
}

int main()
{
  vec_str v; v.push_back("12");
  v.push_back("hello");

  // There are three ways how the functor can be called.

  // 1. By explicitly specifying function name.
  int f_ret = f(12, "qq");
  printf("f(v) == %d\n", f_ret);

  // 2. By creating the instance of a functor.
  Functor_f g;
  int g_ret = g(v);
  printf("%s(v) == %d\n", g.getName().c_str(), g_ret);

  // 3. By searching for functor in a functor map.
  Functor* h = func_map("f");
  int h_ret = h->call(v);
  printf("%s(v) == %d\n", h->getName().c_str(), h_ret);

  return 0;
}

