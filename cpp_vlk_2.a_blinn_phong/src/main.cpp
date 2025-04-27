#include <fmt/format.h>  // fmt::print

#include <iostream>

#include "app.hpp"

using fmt::print;

int main() {
    App app;

    try {
        app.run();
    } catch (const std::exception& e) {
        print(stderr, "exception: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}