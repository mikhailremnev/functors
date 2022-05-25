#ifndef FUNCTOR_H
#define FUNCTOR_H

#include <map>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <sstream>

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
  template <typename T>
  Typeless(const T &val)
  {
    std::stringstream ss;
    ss << std::showbase << std::hex << val;
    value = ss.str();
  }
  Typeless(const int &val)
  {
    char buf[255];
    static const int sz = sizeof(buf) / sizeof(char);
    snprintf(buf, sz, "%d", val);
    value = std::string(buf);
  }

  Typeless& operator=(const std::string &s)
  {
    value = s;
    return *this;
  }

  operator const char*() {
    return value.c_str();
  }

  template <typename T>
  operator T() {
    T ret;
    std::stringstream ss;

    ss << std::showbase << value;
    ss >> ret;

    return ret;
  }
  template<typename T>
  operator T*() { return (T*)strtol(value.c_str(), NULL, 0); }
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
  // Return number of arguments required by this metafunction
  int getArgCount() { return arg_count; }
  // Call function
  virtual Typeless operator()(vec_str v) {return 0;}
  Typeless call(vec_str v) { return (*this)(v); }

  void checkArgs(vec_str& arg_vals)
  {
    int vec_size = arg_vals.size();
    if (vec_size < arg_count) {
      if (arg_count == 1 && args.substr(0, 3) == "int") {
        arg_vals.push_back("0");
        return;
      }

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
/*        MACRO TO CREATE NEW FUNCTORS        */
/**********************************************/

//== Macros to define a functor.
//   First argument is a function name, the rest is the list
//   of input arguments. For example:
//   ```
//   FUNCTOR(my_func, int x, const char* s)
//   {
//     std::cout << s << std::endl;
//     return x * x;
//   }
//   ```
//   This will define Functor_my_func class and create its
//   instance in func_map.
//
//== Technical notes
//   ##__VA_ARGS__ is a non-standard way of reducing (, ##__VA_ARGS__) to ()
//   if the number of arguments is zero.
//   It is supported by most modern compilers but in case it is not available,
//   this alternative can be used:
//   https://stackoverflow.com/questions/5588855/standard-alternative-to-gccs-va-args-trick
//   (basically, use arg counting, similar to __FUNCTOR_CREATE_DECL)
#define FUNCTOR(funcname, ...) \
  Typeless funcname(__FUNCTOR_DISCARD_FIRST_ARG(, ##__VA_ARGS__, FUNCTOR_ARG_LIST_DECL)); \
  class Functor_ ## funcname : public Functor \
  { \
  public: \
    Functor_ ## funcname() : Functor(#funcname, #__VA_ARGS__) {} \
    Functor_ ## funcname(const Functor& copy) : Functor(copy) {} \
    virtual Typeless operator()(vec_str v) \
    { \
      checkArgs(v); \
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
  Typeless funcname(__FUNCTOR_DISCARD_FIRST_ARG(, ##__VA_ARGS__, FUNCTOR_ARG_LIST_IMPL))

//------------------------
// Creating functors for existing functions/constructors/methods
//------------------------

//== Create functor from existing constructor.
//   Example:
//   namespace functor
//   {
//     using namespace std;
//     FUNCTOR_FROM_CONSTRUCTOR(string, const char*);
//   }
//   will make constructor of std::string available as
//   functor::string_constructor(const char* arg1)
//
//== Technical notes
//   classname should not contain colons, thus it is
//   necessary to use typedefs or `using namespace` declaration.
//   Otherwise, functor name will not be valid.
#define FUNCTOR_FROM_CONSTRUCTOR(classname, ...) \
  __FUNCTOR_FROM_CONSTRUCTOR2(classname ## _ ## constructor __FUNCTOR_CREATE_DECL(__VA_ARGS__)) \
  { \
    return new classname(__FUNCTOR_CREATE_CALL(__VA_ARGS__)); \
  }
// Helper function so that arguments are evaluated before the expansion of FUNCTOR macros
#define __FUNCTOR_FROM_CONSTRUCTOR2(...) \
  FUNCTOR(__VA_ARGS__)

//== Create functor from existing method.
//   Example:
//   namespace functor
//   {
//     using namespace std;
//     FUNCTOR_FROM_METHOD(string, string, size_t, size_t)
//     //                  ^^^^^^  ^^^^^^  ^^^^^^^^^^^^^^
//     //               classname  return    arguments
//     //                           type 
//   }
//   will make string::substr available as
//   functor::string_substr(size_t arg1, size_t arg2)
//
//== Technical notes
//   classname should not contain colons, thus it is
//   necessary to use typedefs or `using namespace` declaration.
//   Otherwise, functor name will not be valid.
//   (TODO: It is very easy to add variation of this macros to explicitly specify functor name)
#define FUNCTOR_FROM_METHOD(classname, ret_type, method, ...) \
  FUNCTOR(classname ## _ ## method, void* ptr  __FUNCTOR_CREATE_DECL(__VA_ARGS__)) \
  { \
    classname* obj = (classname*)ptr; \
    __FUNCTOR_RETURN_IF_NOT_VOID(ret_type) obj->method(__FUNCTOR_CREATE_CALL(__VA_ARGS__)); \
    /* If the return type is void, Typeless::None will be returned */ \
    return Typeless::None; \
  }

/**********************************************/
/*             AUXILLARY MACRO                */
/**********************************************/

//== Helper macro to repeat (x) 20 times
#define __FUNCTOR_REPEAT_20(x) \
  x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x

//== List of arguments for declaration and definition (implementation).
//   These macro allow up to 20 input arguments.
#define FUNCTOR_ARG_LIST_DECL \
  __FUNCTOR_REPEAT_20(const Typeless& = Typeless::None)
#define FUNCTOR_ARG_LIST_IMPL \
  __FUNCTOR_REPEAT_20(const Typeless&)


//== Helper function to discard the first argument,
//   as in (a,b,c) |-> (b,c)
//   This is necessary to handle variadic macro with zero arguments in __VA_ARGS__
#define __FUNCTOR_DISCARD_FIRST_ARG(...) __FUNCTOR_DISCARD_FIRST_ARG2(__VA_ARGS__)
#define __FUNCTOR_DISCARD_FIRST_ARG2(_, ...) __VA_ARGS__

//------------------------
// Macro to generate numbered arguments
//------------------------

// Macro based on the number of arguments are taken from this thread:
// http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1

//== Helper macro to create the list of
//   typeN aN, .., type2 a2, type1 a1
//   for the function of N arguments.
#define __FUNCTOR_CREATE_DECL(...) \
    __FUNCTOR_MAP(__FUNCTOR_CREATE_DECL_HELPER, __VA_ARGS__)
#define __FUNCTOR_CREATE_DECL_HELPER(     i, type) , type a ## i
#define __FUNCTOR_CREATE_DECL_HELPER_LAST(i, type) , type a ## i
#define __FUNCTOR_CREATE_DECL_HELPER_NONE(i, type)

//== Helper macro to create the list of
//   aN, .., a2, a1
//   for the function of N arguments.
#define __FUNCTOR_CREATE_CALL(...) \
    __FUNCTOR_MAP(__FUNCTOR_CREATE_CALL_HELPER, __VA_ARGS__)
#define __FUNCTOR_CREATE_CALL_HELPER(     i, type) a ## i,
#define __FUNCTOR_CREATE_CALL_HELPER_LAST(i, type) a ## i
#define __FUNCTOR_CREATE_CALL_HELPER_NONE(i, type)

//== Checker for whether the return type is void or not
//   (will evaluate to " " if it is void
//    and to "return" if it is not void)
#define __FUNCTOR_RETURN_IF_NOT_VOID(x) \
  __FUNCTOR_RETURN_IF_NOT_VOID2(__FUNCTOR_RET_ ## x(), return)
#define __FUNCTOR_RETURN_IF_NOT_VOID2(...) \
  __FUNCTOR_RETURN_IF_NOT_VOID3(__VA_ARGS__)
#define __FUNCTOR_RETURN_IF_NOT_VOID3(arg1, arg2, ...) \
  arg2
#define __FUNCTOR_RET_void(x)  , 

//== Run provided macros on the list of arguments
#define __FUNCTOR_MAP(macros, ...) \
    __FUNCTOR_MAP_HELPER(__FUNCTOR_ARGCOUNT(__VA_ARGS__), macros, __VA_ARGS__)
#define __FUNCTOR_MAP_HELPER(argc, macros, ...) \
  __FUNCTOR_MAP_HELPER2(argc, macros, __VA_ARGS__)
#define __FUNCTOR_MAP_HELPER2(argc, macros, ...) \
    __FUNCTOR_MAP ## argc(macros, __VA_ARGS__)

//== These 20 macro are only called by __FUNCTOR_MAP
//   and apply function f to the list of passed arguments
//   (apply f_LAST to the last argument,
//    apply f_NONE if there were 0 arguments)
#define __FUNCTOR_MAP0( f,    ...) f##_NONE(0 , x)
#define __FUNCTOR_MAP1( f, x, ...) f##_LAST(1 , x)
#define __FUNCTOR_MAP2( f, x, ...) f       (2 , x) __FUNCTOR_MAP1( f, __VA_ARGS__)
#define __FUNCTOR_MAP3( f, x, ...) f       (3 , x) __FUNCTOR_MAP2( f, __VA_ARGS__)
#define __FUNCTOR_MAP4( f, x, ...) f       (4 , x) __FUNCTOR_MAP3( f, __VA_ARGS__)
#define __FUNCTOR_MAP5( f, x, ...) f       (5 , x) __FUNCTOR_MAP4( f, __VA_ARGS__)
#define __FUNCTOR_MAP6( f, x, ...) f       (6 , x) __FUNCTOR_MAP5( f, __VA_ARGS__)
#define __FUNCTOR_MAP7( f, x, ...) f       (7 , x) __FUNCTOR_MAP6( f, __VA_ARGS__)
#define __FUNCTOR_MAP8( f, x, ...) f       (8 , x) __FUNCTOR_MAP7( f, __VA_ARGS__)
#define __FUNCTOR_MAP9( f, x, ...) f       (9 , x) __FUNCTOR_MAP8( f, __VA_ARGS__)
#define __FUNCTOR_MAP10(f, x, ...) f       (10, x) __FUNCTOR_MAP9( f, __VA_ARGS__)
#define __FUNCTOR_MAP11(f, x, ...) f       (11, x) __FUNCTOR_MAP10(f, __VA_ARGS__)
#define __FUNCTOR_MAP12(f, x, ...) f       (12, x) __FUNCTOR_MAP11(f, __VA_ARGS__)
#define __FUNCTOR_MAP13(f, x, ...) f       (13, x) __FUNCTOR_MAP12(f, __VA_ARGS__)
#define __FUNCTOR_MAP14(f, x, ...) f       (14, x) __FUNCTOR_MAP13(f, __VA_ARGS__)
#define __FUNCTOR_MAP15(f, x, ...) f       (15, x) __FUNCTOR_MAP14(f, __VA_ARGS__)
#define __FUNCTOR_MAP16(f, x, ...) f       (16, x) __FUNCTOR_MAP15(f, __VA_ARGS__)
#define __FUNCTOR_MAP17(f, x, ...) f       (17, x) __FUNCTOR_MAP16(f, __VA_ARGS__)
#define __FUNCTOR_MAP18(f, x, ...) f       (18, x) __FUNCTOR_MAP17(f, __VA_ARGS__)
#define __FUNCTOR_MAP19(f, x, ...) f       (19, x) __FUNCTOR_MAP18(f, __VA_ARGS__)

//== Helper macro to count the number of arguments
//   in variadic templates.
//   (Note that ##__VA_ARGS__ is non-standard extension provided by GCC)
//   (without it, we won't be able to handle the case with 0 arguments)
#define __FUNCTOR_ARGCOUNT(...) \
    __FUNCTOR_ARGCOUNT2(_, ##__VA_ARGS__, __FUNCTOR_RSEQ())
#define __FUNCTOR_ARGCOUNT2(_, ...) \
    __FUNCTOR_ARGCOUNT3(__VA_ARGS__)
#define __FUNCTOR_ARGCOUNT3( \
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,     \
    N, ...) N

// Used for argument counting
#define __FUNCTOR_RSEQ() \
    19,18,17,16,15,14,13,12,11,10, \
     9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#endif // FUNCTOR_H

