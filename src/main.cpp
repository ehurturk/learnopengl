#include "Engine.hpp"

int main(int argc, char **argv) {
    Engine engine((EngineConfig){
            .title  = "Mandelbrot Set Visualizer",
            .width  = 800,
            .height = 800});
    engine.start();
}
