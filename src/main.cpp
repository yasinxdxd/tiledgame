#include <windows.h>
#define YT2D_NO_STDINT
#include <Vector.hh>
#include <Window.hpp>
#include <texture2d.hpp>
#include <render_texture2d.hpp>
#define SHAPE_QUAD
#define SHAPE_CUBE
#include <renderables.hpp>
#include <shader_compiler.hpp>
#include <shader.hpp>
#include <input.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <sstream>
#include <TextEditor.h>
#include <game.hpp>
#include <font.hpp>
#include <glfw/glfw3.h>
#include <chrono>
#include <thread>
#include <time.h>

extern "C" {
    #include <tvm/tvm.h>
}


bool darkMode = false;

ImFont* fontBig;
ImFont* fontSml;

static TextEditor editor;
std::string code_buffer_str;

std::string _log;

Shader* shader;
Camera cam;
Texture2D* texture;
RenderTexture2D renderTexture;

Texture2D* textureTarget;
RenderTexture2D renderTextureTarget;

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;
float progresses[11*11*11] = {0.f};

float rnds[11*11*11];

bool read_file_to_string(const std::string& path, std::string& out) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    if (size < 0) return false;
    file.seekg(0, std::ios::beg);

    out.resize(size);
    if (!file.read(&out[0], size)) return false;
    return true;
}

bool save_string_to_file(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;

    file.write(content.c_str(), content.size());
    return file.good();
}


void ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
        *v = !*v;
    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = *v ? IM_COL32(145+20, 211, 68+20, 255) : IM_COL32(218-20, 218-20, 218-20, 255);
    else
        col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

void InitEditor() {
    editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
    editor.SetText(code_buffer_str);
    editor.SetHandleKeyboardInputs(true);
    editor.SetPalette(editor.GetLightPalette());
}

std::string RunCommandAndCaptureOutput(const char* command) {
    std::string result;
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) return "Failed to create pipe\n";

    // Ensure the write handle to the pipe for STDOUT is not inherited.
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    STARTUPINFOW si = { sizeof(si) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWrite;
    si.hStdError  = hWrite;
    si.hStdInput  = NULL;

    std::wstring wcommand = std::wstring(L"cmd.exe /C ") + std::wstring(command, command + strlen(command));

    if (!CreateProcessW(NULL, &wcommand[0], NULL, NULL, TRUE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(hWrite);
        CloseHandle(hRead);
        return "Failed to run command\n";
    }

    CloseHandle(hWrite);

    char buffer[256];
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = 0;
        result += buffer;
    }

    CloseHandle(hRead);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

void RenderAnsiColoredText(const std::string& ansiText) {
    std::regex ansiRegex(R"(\x1B\[([0-9;]+)m)");
    std::smatch match;

    ImVec4 currentColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    std::map<int, ImVec4> colorMap = {
        {30, ImVec4(0, 0, 0, 1)},     // Black
        {31, ImVec4(1, 0, 0, 1)},     // Red
        {32, ImVec4(0, 1, 0, 1)},     // Green
        {33, ImVec4(1, 1, 0, 1)},     // Yellow
        {34, ImVec4(0, 0, 1, 1)},     // Blue
        {35, ImVec4(1, 0, 1, 1)},     // Magenta
        {36, ImVec4(0, 1, 1, 1)},     // Cyan
        {37, ImVec4(1, 1, 1, 1)},     // White
        {90, ImVec4(0.5f, 0.5f, 0.5f, 1)}, // Bright Black (Gray)
    };

    std::string::const_iterator searchStart(ansiText.cbegin());
    std::string buffer;

    while (std::regex_search(searchStart, ansiText.cend(), match, ansiRegex)) {
        buffer += std::string(searchStart, searchStart + match.position());

        // Split and print buffered lines
        std::istringstream stream(buffer);
        std::string line;
        while (std::getline(stream, line)) {
            ImGui::TextColored(currentColor, "%s", line.c_str());
        }
        buffer.clear();

        // Parse all semicolon-separated attributes
        std::string codesStr = match[1];
        std::istringstream codeStream(codesStr);
        std::string code;
        while (std::getline(codeStream, code, ';')) {
            int val = std::stoi(code);
            if (val == 0) {
                currentColor = ImGui::GetStyleColorVec4(ImGuiCol_Text); // reset
            } else if (colorMap.count(val)) {
                currentColor = colorMap[val];
            }
            // You can handle bold (1), underline (4), etc. here if needed
        }

        searchStart = match.suffix().first;
    }

    // Final flush
    buffer += std::string(searchStart, ansiText.cend());
    std::istringstream stream(buffer);
    std::string line;
    while (std::getline(stream, line)) {
        ImGui::TextColored(currentColor, "%s", line.c_str());
    }
}

void RenderEditor(yt2d::Window& window, std::vector<GameObj>& objects) {
    ImGui::Begin("code", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImVec2(window.getWindowWidth() / 2, window.getWindowHeight() * 4/5.0));
    editor.Render("Editor");

    ImGui::End();
    ImGui::Begin("compile", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::SetWindowSize(ImVec2(window.getWindowWidth() / 2, 50));
    ImGui::SetWindowPos(ImVec2(window.getWindowWidth() / 2, 0));
    ImGui::PushFont(fontSml);
    if (ImGui::Button("run", ImVec2(48, 36))) {
        save_string_to_file("tiledgame.tile", editor.GetText());
        
        _log = RunCommandAndCaptureOutput("tile tiledgame.tile -o tiledgame");
        memset(progresses, 0.f, sizeof(float) * 11*11*11);
        
        // check compiler err
         if (_log.find("ERROR") == std::string::npos) {
             
            for (size_t y = 0; y < 11; y++) {
                for (size_t x = 0; x < 11; x++) {
                    for (size_t z = 0; z < 11; z++) {
                        tvm_t* vm = tvm_init();
                        tvm_load_program_from_file(vm, "tiledgame.bin");
                        vm->gframe->global_vars[0].i32 = (x - 5);
                        vm->gframe->global_vars[1].i32 = (y - 5);
                        vm->gframe->global_vars[2].i32 = (z - 5);

                        tvm_run(vm);
                        
                        auto& obj = objects[y * 121 + x * 11 + z];
                        obj.color = vm->stack[vm->sp - 1].ui32;
                        tvm_destroy(vm);

                        rnds[y * 121 + x * 11 + z] = (rand() % 50) / 100.f + 0.7f;
                    }
                }
            }
            
            // std::cout << vm->gframe->global_vars[0].i32 << std::endl;
        }

        // TODO: check if objectstarget and objects are matching!

    }

    ImGui::PopFont();
    ImGui::End();

    ImGui::Begin("console", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::SetWindowPos(ImVec2(0, window.getWindowHeight() * 4/5.0));
    ImGui::SetWindowSize(ImVec2(window.getWindowWidth() / 2, window.getWindowHeight() * 1/5.0));

    ImGui::BeginChild("Compiler Output");
    ImGui::PushFont(fontSml);
    RenderAnsiColoredText(_log);
    ImGui::PopFont();
    ImGui::EndChild();

    ImGui::End();
}

std::tuple<float, float, float, float> colorFromUInt(uint32_t color) {
    float a = ((color >> 24) & 0xFF) / 255.0f; // Alpha component
    float b = ((color >> 16) & 0xFF) / 255.0f; // Blue component
    float g = ((color >> 8)  & 0xFF) / 255.0f; // Green component
    float r = (color         & 0xFF) / 255.0f; // Red component
    return {r, g, b, a};
}


void RenderGame(yt2d::Window& window, std::vector<GameObj>& objects, std::vector<GameObj>& objectsTarget, Camera& cam) {

    // game
    renderTexture.bind();
    glViewport(0, 0, renderTexture.get_texture()->getWidth(), renderTexture.get_texture()->getHeight());
    if (darkMode) window.clear(0.1, 0.15, 0.2, 1);
    else window.clear(0.9, 0.9, 0.9, 1);

    for (size_t y = 0; y < 11; y++) {
        for (size_t x = 0; x < 11; x++) {
            for (size_t z = 0; z < 11; z++) {
                auto& obj = objects[y * 121 + x * 11 + z];
                auto [r, g, b, a] = colorFromUInt(obj.color);
                obj.setPos(glm::vec3(float(x - 5.0) * 2.f, float(y - 5.0) * 2.f, float(z - 5.0) * 2.f));
                // obj.setPos(glm::vec3(0));
                if (obj.color != 0) {
                    render(obj.cube, 36, shader, [&](Shader* shader) {
                        float& progress = progresses[y * 121 + x * 11 + z];
                        if (progress < 1.0f) {
                            obj.model = glm::scale(obj.model, glm::vec3(progress));
                            progress += (deltaTime + rnds[y * 121 + x * 11 + z]) * 0.1; // or some time-based increment
                            // std::cout << progress << std::endl;
                        } else {
                            progress = 1.f;
                        }

                        shader->set<float, 1>("u_progress", progress);
                        shader->set_matrix("m_model", obj.model);
                        shader->set_matrix("m_view", cam.view);
                        shader->set_matrix("m_projection", cam.projection);
                        shader->set<float, 4>("u_color", r, g, b, a);

                    }, GL_TRIANGLES);
                }
            }
        }
    }
    renderTexture.unbind();


    // target
    renderTextureTarget.bind();
    glViewport(0, 0, renderTextureTarget.get_texture()->getWidth(), renderTextureTarget.get_texture()->getHeight());
    if (darkMode) window.clear(0.1, 0.15, 0.2, 1);
    else window.clear(0.9, 0.9, 0.9, 1);

    for (size_t y = 0; y < 11; y++) {
        for (size_t x = 0; x < 11; x++) {
            for (size_t z = 0; z < 11; z++) {
                auto& obj = objectsTarget[y * 121 + x * 11 + z];
                auto [r, g, b, a] = colorFromUInt(obj.color);
                obj.setPos(glm::vec3(float(x - 5.0) * 2.f, float(y - 5.0) * 2.f, float(z - 5.0) * 2.f));
                // obj.setPos(glm::vec3(0));
                if (obj.color != 0) {
                    render(obj.cube, 36, shader, [&](Shader* shader) {
                        shader->set<float, 1>("u_progress", 1.f);
                        shader->set_matrix("m_model", obj.model);
                        shader->set_matrix("m_view", cam.view);
                        shader->set_matrix("m_projection", cam.projection);
                        shader->set<float, 4>("u_color", r, g, b, a);
                    }, GL_TRIANGLES);
                }
            }
        }
    }
    renderTextureTarget.unbind();


    ImGui::Begin("game", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::SetWindowSize(ImVec2(window.getWindowWidth() / 2, window.getWindowHeight() - 50));
    ImGui::SetWindowPos(ImVec2(window.getWindowWidth() / 2, 50));

    {
        ImTextureID textureID = (ImTextureID)(intptr_t)((unsigned int)(*renderTexture.get_texture()));
        ImGui::Text("Display");
        ImVec2 size = ImVec2(window.getWindowWidth() / 2 - 120, window.getWindowHeight() / 2 - 100);
        ImGui::Image(textureID, size, ImVec2(0, 1), ImVec2(1, 0)); // Flip UVs for correct orientation
    }

    {
        ImTextureID textureID = (ImTextureID)(intptr_t)((unsigned int)(*renderTextureTarget.get_texture()));
        ImGui::Text("Target");
        ImVec2 size = ImVec2(window.getWindowWidth() / 2 - 120, window.getWindowHeight() / 2 - 100);
        ImGui::Image(textureID, size, ImVec2(0, 1), ImVec2(1, 0));
    }
    
    ImGui::BeginChild("dark-mode-child");
        ImGui::PushFont(fontSml);
        ImGui::Text("dark-mode");
        ImGui::SameLine();
        ToggleButton("dark-mode", &darkMode);
        if (darkMode) {
            editor.SetPalette(editor.GetDarkPalette());
            ImGui::StyleColorsDark();
        } else {
            editor.SetPalette(editor.GetLightPalette());
            ImGui::StyleColorsLight();
        }
        ImGui::PopFont();
        ImGui::EndChild();
    ImGui::End();
}

void InitImgui(yt2d::Window& window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    fontBig = ImGui::GetIO().Fonts->AddFontFromFileTTF("JetBrainsMonoNL-Light.ttf", 32.0f);
    fontSml = ImGui::GetIO().Fonts->AddFontFromFileTTF("JetBrainsMonoNL-Light.ttf", 18.0f);
    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cout << "ERROR: ImGui_ImplGlfw_InitForOpenGL" << std::endl;
    }
    #ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
    #endif
    if (!ImGui_ImplOpenGL3_Init("#version 330 core")) {
        std::cout << "ERROR: ImGui_ImplOpenGL3_Init" << std::endl;
    }

    glfwSetScrollCallback(window, _priv::callbacks::scroll_callback);
}

void DestroyImgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int main(int argc, char* argv[])
{
    yt2d::Window window("Tiled", 1280, 720);
    read_file_to_string("tiledgame.tile", code_buffer_str);

    srand(time(NULL));

    InitImgui(window);
    InitEditor();
    
    glcompiler::init();
    shader = new Shader();
    shader->load_shader_code("shaders/default_colored_rect.vert", Shader::ShaderCodeType::VERTEX_SHADER);
    shader->load_shader_code("shaders/batch_renderer.frag", Shader::ShaderCodeType::FRAGMENT_SHADER);
    glcompiler::compile_and_attach_shaders(shader);

    texture = new Texture2D(640, 480, nullptr);
    texture->generate_texture();
    renderTexture.set_texture(texture);

    textureTarget = new Texture2D(640, 480, nullptr);
    textureTarget->generate_texture();
    renderTextureTarget.set_texture(textureTarget);
    

    // init_free_type();

    // Font font3d;
    // font3d.load_font("./Ubuntu-Title.ttf");
    // Texture2D* fontAtlas = font3d.create_font_atlas();
    // Quad textQuad;

    // Shader* textShader = new Shader();
    // textShader->load_shader_code("shaders/default_colored_rect.vert", Shader::ShaderCodeType::VERTEX_SHADER);
    // textShader->load_shader_code("shaders/batch_renderer.frag", Shader::ShaderCodeType::FRAGMENT_SHADER);
    // glcompiler::compile_and_attach_shaders(textShader);


    std::vector<GameObj> objects(11*11*11);
    std::vector<GameObj> objectsTarget(11*11*11);

    const float targetFrameTime = 1.0f / 60.0f;

    while (!window.isClose()) {

        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        window.pollEvent();

        cam.move(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
        RenderGame(window, objects, objectsTarget, cam);
        RenderEditor(window, objects);
            
        ImGui::Render();
        
        if (darkMode) window.clear(0.1, 0.15, 0.2, 1);
        else window.clear(0.9, 0.9, 0.9, 1);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // fontAtlas->bind();
        // render(textQuad, 6, shader, [&](Shader* shader) {
        //     shader->set_matrix("m_model", glm::mat4(1.0));
        //     shader->set_matrix("m_view", glm::mat4(1.0));
        //     shader->set_matrix("m_projection", cam.projection);
            // shader->set<float, 4>("u_color", 1, 1, 1, 1);
            // shader->set<int, 1>("u_texture", 0);
        // }, GL_TRIANGLES);
        // fontAtlas->unbind();


        window.display();


        // --- Frame limiting ---
        float frameTime = glfwGetTime() - currentFrameTime;
        if (frameTime < targetFrameTime) {
            float sleepTime = targetFrameTime - frameTime;
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
        }
    }


    // destroy_free_type();
    delete shader;
    delete texture;
    delete textureTarget;
    DestroyImgui();

    return 0;
}
