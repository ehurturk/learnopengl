#include <queue>

#include <glad/glad.h>

#include "Renderable2D.h"

class Renderer2D
{
public:
	Renderer2D();
	~Renderer2D();

	void AddToRenderQueue(Renderable2D* renderable)
	{
		m_Renderables.push(renderable);
	}

	void DrawAll()
	{
		
	}
private:
	std::queue<Renderable2D*> m_Renderables;
};