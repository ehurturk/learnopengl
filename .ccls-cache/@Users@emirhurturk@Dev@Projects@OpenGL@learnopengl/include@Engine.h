#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Renderer2D.h>

struct EngineConfig
{
	const char *title;
	int width;
	int height;
};

class Engine
{
public:
	Engine(const EngineConfig config);
	~Engine();

	void start();
	void mainloop();
private:
	std::unique_ptr<Renderer2D> renderer2D;
};