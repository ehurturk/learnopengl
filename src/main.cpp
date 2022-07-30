#include "Engine.hpp"
#include "../apps/SponzaApp.hpp"
#include "../apps/Mandelbrot.hpp"

int main(int argc, char **argv) {
    Engine &engine = Engine::Get();

    Mandelbrot *app = new Mandelbrot();
    AppConfig cfg   = app->get_config();

    engine.create(cfg.title, cfg.width, cfg.height);
    engine.register_app(app);

    Engine::Get().start();
    Engine::Get().update();

    delete app;
    return EXIT_SUCCESS;
}
