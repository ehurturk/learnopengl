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
8) MAJOR BAG ALERT: 
   1) Now I have the default framebuffer and the other render framebuffer
   2) Hence, I render everything to the framebuffer texture, and I use the texture in ImGui
   3) Since post processing is a screen-space effect, I can't add it to ImGui directly (I can add only if ImGui is not being used with a simple fullscreen quad and a shader and the texture binding)
   4) However, I can create another framebuffer and texture with the same width and height of the viewport, and then render the entire texture into it with it's own post processing shader and then render ImGui::Image passing the 2nd texture value with its shaders.
9) STBI FLIP VERTICALLY ON TRUE GETTING OUT OF HAND

!!!!!!! very very IMPORTANTTT: Framebuffer size of viewport changes when the window is moved.

****************************
10) VERY IMPORTANT!!!!!!!!!! MANAGE APP MEMBER FUNCTION SEG FAULT??
11) MATERIAL CLASS AND SHADER!!
    1)  e.g: cube.set_material(mat);
*****************************

THIS APP IS FUCKING SLOW WITH --RELEASE BUT OK WITH IMGUI WHY THE FUCK?

ADDED 2 FRAMEBUFFERS AND IT WASNT WORKING AND I DUPED IT AND REMOVED THE ORIGINAL ONE AND IT STARTED WORKING.
I HATE IMGUI.

NOTEEE:
FRAMEBUFFER CHANING.
THERE ARE 3 FRAMEBUFFERS (including the default one):
1) m_Framebuffer
2) m_PostProcessBuffer
3) default
   
!!!!
1) Bind m_Framebuffer
2) Render the whole scene into m_Framebuffer
3) Bind m_PostProcessBuffer
4) Get the texture of m_Framebuffer, draw it into a screen-filled quad in NDC coordinates and apply post processing shader found in res/shaders/post_processing.glsl
if (IMGUI) then ImGui::Draw(m_PostProcess.texture);
5) Bind default framebuffer
if (NOT IMGUI) then get the texture of m_PostProcessBuffer, draw it into a screen-filled quad in NDC coords to the final render.

IDK MAN THIS WHOLE IMGUI THING SEEMS LIKE A TRASH

*****************
Scene loading.
Entity (light vs cube?)
Default objects and add texture? (e.g. instantiate Entity::Cube and later add a simple diffuse texture)
Set post processing stack.
Material.
*****************