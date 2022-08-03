# Current issues that don't affect build but are shitty

1) No entity class
2) No framebuffer class
3) Event callbacks are kinda messed up but they work (ZHANG!)
4) Raw thing is also good but im not so sure why lmao
5) also in window.cpp glfwGetFramebufferSize (for OpenGL's glViewport)
6) camera class is a huge shit rn
7) Code design
   1) Zmangus WindowConfig and AppConfig are the same fucking thing then why 2 of them?
   2) Also on framebuffer init, use m_Window->config.width and height since they are initialized with glfwGetFramebufferSize otherwise it is fucked up
   