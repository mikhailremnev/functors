#ifndef FUNCTOR_H
#define FUNCTOR_H

#include <map>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

// See README.md for the detailed description.
// See example.cc for usage example.

/**********************************************/
/*               TYPELESS CLASS               */
/**********************************************/

/**
 * 'Typeless' is a wrapper for the arbitrary return
 * value (since our functors can return anything)
 * The contained value can be implicitly converted
 * to most of the basic types.
 */
class Typeless
{
public:
  static const Typeless None;

  Typeless() { value = "0"; }
  Typeless(const std::string &val) { value = val; }
  Typeless(const char *val) { value = val; }
  Typeless(const int &val)
  {
    char buf[255];
    static const int sz = sizeof(buf) / sizeof(char);
    snprintf(buf, 255, "%d", val);
    value = std::string(buf);
  }

  Typeless& operator=(const std::string &s)
  {
    value = s;
    return *this;
  }

  operator int() { return atoi(value.c_str()); }
  operator double() { return atof(value.c_str()); }
  operator float() { return atof(value.c_str()); }
  operator const char*() { return value.c_str(); }
  template<typename T>
  operator T*() { return (T*)atol(value.c_str()); }
  operator std::string() { return value; }

private:
  std::string value;
};

const Typeless Typeless::None = Typeless();

/**********************************************/
/*               FUNCTOR CLASS                */
/**********************************************/

typedef std::vector<std::string> vec_str;

/**
 * 'Functor' is a wrapper class for the arbitrary function.
 * All functors are added to `func_map` global variable and
 * can be searched by name.
 */
class Functor
{
public:
  Functor() : name(""), args("") { arg_count = 0; }
  Functor(const char *_name, const char *_args) : name(_name), args(_args)
  {
    // Number of arguments ~= number of commas + 1
    // NOTE: This won't work on cases like std::map<int, int>
    arg_count = 1 + std::count(args.begin(), args.end(), ',');
  }
  Functor(const Functor &copy) : name(copy.name), args(copy.args), arg_count(copy.arg_count) {}

  // Returns function name
  std::string getName() { return name; }
  // Returns something like "int x, int y, float z"
  std::string getArgs() { return args; }
  // Call function
  virtual Typeless operator()(vec_str v) {return 0;}
  Typeless call(vec_str v) { return (*this)(v); }

  void checkArgCount(int vec_size)
  {
    if (vec_size < arg_count) {
      char buf[256];
      const int sz = sizeof(buf) / sizeof(char);
      snprintf(buf, sz, "Not enough arguments passed to function %s(%s): "
          "expected %d, got %d",
          getName().c_str(), args.c_str(), arg_count, vec_size);
      throw std::invalid_argument(buf);
    }
  }

private:
  /** Function name */
  std::string name;
  /** List of function arguments, e.g. "int x, int y, float z" */
  std::string args;
  /** Number of arguments */
  int arg_count;
};

/** Named map of all functors created with FUNCTOR macro */
std::map<std::string, Functor*> func_map;

/**********************************************/
/*              MACRO DEFINITIONS             */
/**********************************************/

//== List of arguments for declaration and definition (implementation).
//   These macro allow up to 20 input arguments.
#define FUNCTOR_ARG_LIST_DECL \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None, \
  const Typeless& = Typeless::None, const Typeless& = Typeless::None
#define FUNCTOR_ARG_LIST_IMPL \
  const Typeless&, const Typeless&, const Typeless&, const Typeless&, \
  const Typeless&, const Typeless&, const Typeless&, const Typeless&, \
  const Typeless&, const Typeless&, const Typeless&, const Typeless&, \
  const Typeless&, const Typeless&, const Typeless&, const Typeless&, \
  const Typeless&, const Typeless&, const Typeless&, const Typeless&

//== Macro to define a functor.
//   First argument is a function name, the rest is the list
//   of input arguments. For example:
//   ```
//   FUNCTOR(my_func, int x, const char* s)
//   { ... implementation ... }
//   ```
//   This will define Functor_my_func class and create its
//   instance in func_map.
#define FUNCTOR(funcname, ...) \
  Typeless funcname(__VA_ARGS__, FUNCTOR_ARG_LIST_DECL); \
  class Functor_ ## funcname : public Functor \
  { \
  public: \
    Functor_ ## funcname() : Functor(#funcname, #__VA_ARGS__) {} \
    Functor_ ## funcname(const Functor& copy) : Functor(copy) {} \
    virtual Typeless operator()(vec_str v) \
    { \
      checkArgCount(v.size()); \
      Typeless args[20]; \
      for (unsigned int i = 0; i < 20 && i < v.size(); i++) { \
        args[i] = v[i]; \
      } \
      \
      return funcname( \
        args[ 0], args[ 1], args[ 2], args[ 3], args[ 4], args[ 5], \
        args[ 6], args[ 7], args[ 8], args[ 9], args[10], args[11], \
        args[12], args[13], args[14], args[15], args[16], args[17], \
        args[18], args[19]); \
    } \
  }; \
  Functor_ ## funcname * funcname ## _ptr = (Functor_ ## funcname *)(func_map[#funcname] = new Functor_ ## funcname()); \
  Typeless funcname(__VA_ARGS__, FUNCTOR_ARG_LIST_IMPL)


#endif // FUNCTOR_H

