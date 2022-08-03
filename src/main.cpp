#include "Engine.hpp"
#include "../apps/SponzaApp.hpp"
#include "../apps/Mandelbrot.hpp"
#include <cstdlib>
#include <memory>

/* usage: ./LearnOpenGL --raw (for displaying only the game)*/
/*    or: ./LearnOpenGL (for displaying the editor)*/
int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "usage: ./LearnOpenGL (--raw)/(-r)\n"
                  << "or ./LearnOpenGL"
                  << std::endl;
        return EXIT_FAILURE;
    }

    Engine &engine = Engine::Get();

    auto app      = std::make_shared<SponzaApp>();
    AppConfig cfg = app->get_config();
    if (argc == 2 && (strcmp(argv[1], "--raw") == 0 || strcmp(argv[1], "-r") == 0)) {
        cfg.raw = true;
    }

    engine.create(cfg);
    engine.register_app(app.get());

    Engine::Get().start();
    Engine::Get().update();

    return EXIT_SUCCESS;
}
