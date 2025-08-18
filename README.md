# Learning OpenGL

This repository documents my journey of (hopefully) learning graphics programming. The textbook I'm reading is V. Scott Gordon, and John Clevenger, _Computer Graphics Programming in OpenGL with C++_, for more information see [references](#references).

I'm using C++23 with [modules](https://en.cppreference.com/w/cpp/language/modules.html), the [CMake](https://cmake.org) build system, and the [vcpkg](https://github.com/microsoft/vcpkg) package manager.

## Building

1. Install CMake 3.28 or newer
2. [Set up vcpkg](https://learn.microsoft.com/en-us/vcpkg/)
3. Clone the repository and build the project

```sh
git clone http://github.com/faresbakhit/gl && cd gl
cmake --preset release
cmake --build --preset release
```

## References

1. V. Scott Gordon, and John Clevenger, _Computer Graphics Programming in OpenGL with C++_, 3rd ed. (Mercury Learning & Information, 2024).
