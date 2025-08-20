import lib.window;
#include <cstdlib>
#include <exception>
#include <print>

int main()
{
    try {
        lib::Window window(640, 480, "hello world");
        window.run();
    } catch (std::exception const& e) {
        std::println("[ERROR] {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
