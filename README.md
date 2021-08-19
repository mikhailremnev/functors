Named functor library.

The idea is to have an extremely simple, header-only library that can provide a
minimalistic support for named functions that can be integrated with an interpreter.

For example, to create a named function:
```
FUNCTOR(my_func, int x, float y, double z)
{
  printf("x, y, z = %d, %f, %lf", x, y, z);
}
```

Then, this function can be called in multiple different ways, as demonstrated in example.cc
The arguments are also correctly converted from string to the correct type.

