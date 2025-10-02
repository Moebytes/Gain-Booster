# C++ Style Guide

If you want to contribute to my repository, I ask that you follow my code styling. If something doesn't appear in this guide then I have no specific preference 
for it (but this may get updated in the future as needed).

**1. File Extension** \
Cpp files should use `.cpp` and header files `.h`. Combined header and source files 
should use `.hpp`. 

If a file is small enough you should combine the header and source into one.

**2. Naming Scheme** \
Variables and functions use `camelCase`. Classes and structures use `PascalCase`. 

```cpp
// x Bad
class aClass {};
auto Method() -> void {}
auto Hi = 1;
// ✓ Good
class AClass {};
auto method() -> void {}
auto hi = 1;
```

**3. Braces** \
Always put the function brace on the same line.

```cpp
// x Bad
auto main() -> int
{
}
// ✓ Good
auto main() -> int {
}
```

**4. Return types at the end** \
Always put the return type at the end. This nicely left aligns all functions, useful for long return types.

```cpp
// x Bad
std::optional<std::pair<std::string, double>> Class::myFunc() {}
// ✓ Good
auto Class::myFunc() -> std::optional<std::pair<std::string, double>> {}
```

**5. Use auto** \
Always use auto, unless it is a basic type (int, double), or you don't 
need to pass arguments to the class constructor, or you want an implicit type
conversion.

```cpp
// x Bad
std::unique_ptr<MyClass> myClass = std::make_unique<MyClass>();
// ✓ Good
auto myClass = std::make_unique<MyClass>();
int x = 5;
double y = 8.0;
MyClass myClass;
std::string str = "";
```

**6. Use auto\* for pointer types** \
When the type is a pointer, make sure you use auto\*. 

```cpp
// x Bad
auto myClass = new MyClass{};
// ✓ Good
auto* myClass = new MyClass{};
```

**7. Use smart pointers when possible** \
Avoid raw pointers, unless you are being limited by a library.

```cpp
// x Bad
auto* myClass = new MyClass{};
// ✓ Good
auto myClass = std::make_unique<MyClass>();
```

**8. Use brace initialization for classes, else copy initialization** 

```cpp
// x Bad
int x{5};
// ✓ Good
int x = 5;
auto myClass = MyClass{5, 4};
```

**9. Pointers and reference are part of the type** 

```cpp
// x Bad
auto method(const std::string &str, float *ptr) -> void {}
// ✓ Good
auto method(const std::string& str, float* ptr) -> void {}
```

**10. Prefer auto function parameters, unless the template is needed in the function body** 

```cpp
// x Bad
template <typename F>
auto bind(F&& func, const std::string& name) -> void {}
// ✓ Good
auto bind(auto&& func, const std::string& name) -> void {}
```

**11. Preface class member variables with this** 

```cpp
// x Bad
doSomething();
// ✓ Good
this->doSomething();
```

**12. Use static_cast and dynamic_cast over C casts** \
Also avoid using reinterpret_cast unless necessary.

```cpp
// x Bad
int x = (int) 5.3;
// ✓ Good
int x = static_cast<int>(5.3);
```

**13. Use post increment in for loops** \
There shouldn't be a difference anymore, and post increment looks better.

```cpp
// x Bad
for (int i = 0; i < 5; ++i) {}
// ✓ Good
for (int i = 0; i < 5; i++) {}
```

**14. Use the member initialization list** 

```cpp
// x Bad
MyClass::MyClass(std::string a) {
  this->a = a;
}
// ✓ Good
MyClass::MyClass(std::string a) : a(a) {}
```
