#include "functor.h"

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

  int f_ret = f(12, "qq");
  printf("f(v) == %d\n", f_ret);

  Functor_f g;
  int g_ret = g(v);
  printf("%s(v) == %d\n", g.getName().c_str(), g_ret);

  Functor* h = func_map["f"];
  int h_ret = h->call(v);

  printf("%s(v) == %d\n", h->getName().c_str(), h_ret);

  return 0;
}

