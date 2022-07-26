#include <cstdlib>
#include "Engine.hpp"

int main(int argc, char **argv) {
    if (argc == 1) {
        Engine engine("LearnOpenGL", 800, 600);
        engine.start();
    } else if (argc == 2) {
        Engine engine("LearnOpenGL");
        engine.start();
    } else
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
