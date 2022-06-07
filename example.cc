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

