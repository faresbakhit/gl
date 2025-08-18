module;

#include <print>

export module lib.hello;

export void hello_world() {
    std::println("Hello, world!");
}
