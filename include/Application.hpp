#ifndef APPLICATION_H_
#define APPLICATION_H_

class Application {
public:
    Application() {}
    virtual ~Application() {}

    virtual void start()          = 0;
    virtual void update(float dt) = 0;
    virtual void imgui_update()   = 0;
};


#endif// APPLICATION_H_
