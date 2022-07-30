#include "Engine.hpp"
#include "../apps/SponzaApp.hpp"
#include "../apps/TestApp.hpp"

int main(int argc, char **argv) {
    Engine &engine = Engine::Get();
    SponzaApp *app = new SponzaApp();
    engine.register_app(app);
    Engine::Get().start();
    Engine::Get().update();
    delete app;
    return EXIT_SUCCESS;
}
