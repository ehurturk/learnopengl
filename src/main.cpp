#include "Engine.hpp"
#include "../apps/SponzaApp.hpp"
#include "../apps/Mandelbrot.hpp"
#include <memory>

int main(int argc, char **argv) {
    Engine &engine = Engine::Get();

    // Mandelbrot *app = new Mandelbrot();
    std::shared_ptr<SponzaApp> app = std::make_shared<SponzaApp>();
    AppConfig cfg                  = app->get_config();

    engine.create(cfg.title, cfg.width, cfg.height);
    engine.register_app(app.get());

    Engine::Get().start();
    Engine::Get().update();

    return EXIT_SUCCESS;
}
