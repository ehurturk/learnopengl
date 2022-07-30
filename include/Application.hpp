#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "common.h"

struct AppConfig {
    const char *title;
    ui32 width;
    ui32 height;
};

class Application {
public:
    Application(const char *title, ui32 w, ui32 h) {
        config.title  = title;
        config.width  = w;
        config.height = h;
    }
    virtual ~Application() {}

    virtual void start()          = 0;
    virtual void update(float dt) = 0;
    virtual void imgui_update()   = 0;

    virtual AppConfig get_config() { return config; }

protected:
    AppConfig config;
};


#endif// APPLICATION_H_
