#include "NormalMap.h"

NormalMap::NormalMap() : Application("NormalMap", 800, 600) {}
NormalMap::~NormalMap() {}

void NormalMap::start() {
    // Executed on initialization
    normal_shader.create();
    std::array<const char *, 6> faces = {
        "../res/skyboxes/city/right.jpg",
        "../res/skyboxes/city/left.jpg",
        "../res/skyboxes/city/top.jpg",
        "../res/skyboxes/city/bottom.jpg",
        "../res/skyboxes/city/front.jpg",
        "../res/skyboxes/city/back.jpg"
    };
    stbi_set_flip_vertically_on_load(false);
    box.load_faces(faces);
    stbi_set_flip_vertically_on_load(true);

    normal_shader.load_shader("../res/shaders/normal.glsl");

    normal_shader.set_uniform_block_binding("matrices", 0);

    stbi_set_flip_vertically_on_load(false);
    car.load_model("../res/models/car/source/car.fbx");
    stbi_set_flip_vertically_on_load(true);
}

void NormalMap::update(float dt) {
    // Executed each frame
    normal_shader.use();
    car.draw(normal_shader);

    box.draw();
}

void NormalMap::imgui_update() {
    // Executed each frame for UI
#pragma region STAT WINDOW
    /**================================================== *
     * ==========  Stat Window  ========== *
     * ================================================== */
    float fps = Engine::Get().get_subsystem<Window>()->get_fps(), ms = Engine::Get().get_subsystem<Window>()->get_ms_per_frame();
    static bool p_open            = true;
    static int corner             = 2;
    ImGuiIO &io                   = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1) {
        const float PAD               = 10.0f;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 work_pos               = viewport->WorkPos;// Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size              = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x       = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y       = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f);// Transparent background
    if (ImGui::Begin("Example: Simple overlay", &p_open, window_flags)) {
        ImGui::Text("Stats\n"
                    "(right-click to change position)");
        ImGui::Separator();
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse Position: <invalid>");
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::Text("Time per Frame (ms): %f", ms);
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (&p_open && ImGui::MenuItem("Close")) p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
    /* =======  End of Stat Window  ======= */
#pragma endregion
}
