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
#include <ImGuizmo.h>
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

Shader* confettiShader;

Shader* shader;
Camera cam;
Texture2D* texture;
RenderTexture2D renderTexture;

Texture2D* textureTarget;
RenderTexture2D renderTextureTarget;

Texture2D* texturePostProcess;
RenderTexture2D renderTexturePostProcess;

const float targetFrameTime = 1.0f / 60.0f;
float lastFrameTime = 0.0f;
float deltaTime = 0.0f;
float progresses[11*11*11] = {0.f};

int level = 0;
bool youWinLevel = false;
float winTime = 0.0f;

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

void LoadLevel(std::vector<GameObj>& objectTargets, int level) {
    std::string lvlName = std::string("levels/lvl") + std::to_string(level) + std::string(".bin");
    for (size_t y = 0; y < 11; y++) {
        for (size_t x = 0; x < 11; x++) {
            for (size_t z = 0; z < 11; z++) {
                tvm_t* vm = tvm_init();
                tvm_load_program_from_file(vm, lvlName.c_str());
                vm->gframe->global_vars[0].i32 = (x - 5);
                vm->gframe->global_vars[1].i32 = (y - 5);
                vm->gframe->global_vars[2].i32 = (z - 5);

                tvm_run(vm);
                
                auto& obj = objectTargets[y * 121 + x * 11 + z];
                obj.color = vm->stack[vm->sp - 1].ui32;
                tvm_destroy(vm);

                rnds[y * 121 + x * 11 + z] = (rand() % 50) / 100.f + 0.7f;
            }
        }
    }
}

void YouWinAnimationAndNextLevel(std::vector<GameObj>& objectTargets) {
    if (youWinLevel) {
        float currentTime = glfwGetTime();
        if (currentTime - winTime >= 3.0f) { // Wait for 3 seconds
            LoadLevel(objectTargets, ++level);
            youWinLevel = false;
        } else {
            cam.yaw += 0.0628;
        }
    }
}

void ShowColorSquaresWindow(yt2d::Window& window);

void RenderEditor(yt2d::Window& window, std::vector<GameObj>& objects, std::vector<GameObj>& objectTargets) {
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
        }

        // it checks if all the values are the same!
        if (objectTargets == objects && !youWinLevel) {
            youWinLevel = true;
            winTime = glfwGetTime(); // Store the current time (in seconds)
        }

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

    ImGui::BeginChild("RenderSection", ImVec2(window.getWindowWidth() / 2 - 150, 0), false);
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
    
    ImGui::SameLine();
    ShowColorSquaresWindow(window);

    ImGui::End();
}

std::tuple<float, float, float, float> colorFromUInt(uint32_t color) {
    float a = ((color >> 24) & 0xFF) / 255.0f; // Alpha component
    float b = ((color >> 16) & 0xFF) / 255.0f; // Blue component
    float g = ((color >> 8)  & 0xFF) / 255.0f; // Green component
    float r = (color         & 0xFF) / 255.0f; // Red component
    return {r, g, b, a};
}

ImVec4 IntToImVec4LittleEndian(unsigned int color) {
    auto [r, g, b, a] = colorFromUInt(color);
    return ImVec4(a, b, g, r);
}

void ShowColorSquaresWindow(yt2d::Window& window)
{
    ImGui::BeginChild("ColorSquares", ImVec2(128, 0), true, ImGuiWindowFlags_NoScrollbar);
    {
        struct ColorInfo {
            const char* name;
            ImVec4 color;
        };

        ColorInfo colors[] = {
            {"RED", IntToImVec4LittleEndian(0xbf212fFF)},
            {"GREEN", IntToImVec4LittleEndian(0x27b376ff)},
            {"BLUE", IntToImVec4LittleEndian(0x264b96ff)},
            {"YELLOW", IntToImVec4LittleEndian(0xf9a73eff)},
            {"PINK", IntToImVec4LittleEndian(0xF88379ff)},
            {"EMPTY", darkMode ? IntToImVec4LittleEndian(0x18141400) : IntToImVec4LittleEndian(0xf8f8f800)},
            {"WHITE", IntToImVec4LittleEndian(0xFFFFFFFF)},
            {"BROWN", IntToImVec4LittleEndian(0x964B00FF)},
        };

        ImVec2 square_size(20, 20);

        for (const auto& color : colors)
        {
            ImGui::PushID(color.name);
            ImGui::ColorButton("##color", color.color, ImGuiColorEditFlags_NoTooltip, square_size);
            ImGui::SameLine();
            ImGui::Text("%s", color.name);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", color.name);
            ImGui::PopID();
        }
    }
    ImGui::EndChild();
}

glm::mat4 tmpMat(1.0);
glm::vec3 tmpPos = glm::vec3(0.0, 0.0, 0.0);
glm::mat4 objectMatrix = glm::translate(tmpMat, tmpPos);
void ShowImGuizmoUI(yt2d::Window& window) {
    // ImGuiIO& io = ImGui::GetIO();
    // io.DisplaySize = ImVec2(
    //     (float)renderTexture.get_texture()->getWidth(),
    //     (float)renderTexture.get_texture()->getHeight()
    // );

    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();

    ImGuizmo::BeginFrame();
    ImGui::Begin("game");
    ImGui::BeginChild("RenderSection");
    ImGuizmo::SetDrawlist();
    ImGuizmo::Enable(false);
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetAxisMask(false, false, false);
    // ImGuizmo::SetRect(0, 0 , window.getWindowWidth() , window.getWindowHeight());
    ImVec2 size = ImVec2(window.getWindowWidth() / 4, window.getWindowHeight() / 4);
    ImVec2 offset = ImVec2(window.getWindowWidth() / 24, window.getWindowHeight() / 20);
    ImGuizmo::SetRect(window.getWindowWidth() / 2 - size.x / 2 + offset.x, window.getWindowHeight() / 2 - size.y / 2 - offset.y, size.x, size.y);

    static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
    static ImGuizmo::MODE mode = ImGuizmo::WORLD;

    ImGuizmo::Manipulate(glm::value_ptr(cam.view), glm::value_ptr(cam.projection), operation, mode, glm::value_ptr(objectMatrix));
    ImGui::EndChild();
    ImGui::End();

    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RenderGame(yt2d::Window& window, std::vector<GameObj>& objects, std::vector<GameObj>& objectsTarget, Camera& cam) {

    // game
    renderTexture.bind();
    glViewport(0, 0, renderTexture.get_texture()->getWidth(), renderTexture.get_texture()->getHeight());
    glEnable(GL_DEPTH_TEST);
    if (darkMode) window.clear(0.1, 0.15, 0.2, 1);
    else window.clear(0.9, 0.9, 0.9, 1);

    for (size_t y = 0; y < 11; y++) {
        for (size_t x = 0; x < 11; x++) {
            for (size_t z = 0; z < 11; z++) {
                auto& obj = objects[y * 121 + x * 11 + z];
                auto [r, g, b, a] = colorFromUInt(obj.color);
                obj.setPos(glm::vec3(float(x - 5.0) * 2.f, float(y - 5.0) * 2.f, float(z - 5.0) * 2.f));

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

void RenderAll(yt2d::Window& window, std::vector<GameObj>& objects, std::vector<GameObj>& objectsTarget, Quad& quad) {

    RenderGame(window, objects, objectsTarget, cam);

    renderTexturePostProcess.bind();
    glViewport(0, 0, renderTexturePostProcess.get_texture()->getWidth(), renderTexturePostProcess.get_texture()->getHeight());
    if (darkMode) window.clear(0.1, 0.15, 0.2, 1);
    else window.clear(0.9, 0.9, 0.9, 1);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    RenderEditor(window, objects, objectsTarget);
    ShowImGuizmoUI(window);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    renderTexturePostProcess.unbind();
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



    texturePostProcess = new Texture2D(1920, 1080, nullptr);
    texturePostProcess->generate_texture();
    renderTexturePostProcess.set_texture(texturePostProcess);

    confettiShader = new Shader();
    confettiShader->load_shader_code("shaders/confetti.frag", Shader::ShaderCodeType::FRAGMENT_SHADER);
    glcompiler::compile_and_attach_shaders(confettiShader);

    Quad quad;

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

    LoadLevel(objectsTarget, level);

    while (!window.isClose()) {
        float currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
    
        window.pollEvent();
        cam.move(window);
        
        YouWinAnimationAndNextLevel(objectsTarget);
        RenderAll(window, objects, objectsTarget, quad);


        if (darkMode) window.clear(0.1, 0.15, 0.2, 1);
        else window.clear(0.9, 0.9, 0.9, 1);
            unsigned int quadTexture = ((unsigned int)(*renderTexturePostProcess.get_texture()));
            texturePostProcess->bind();
            render(quad, 6, confettiShader, [&](Shader* shader) {
                shader->set<int, 1>("u_texture", 0);
                shader->set<int, 1>("u_confetti", youWinLevel);
                shader->set<float, 1>("iTime", glfwGetTime());
            }, GL_TRIANGLES);
            texturePostProcess->unbind();
        window.display();



        // frame limiting
        float frameTime = glfwGetTime() - currentFrameTime;
        if (frameTime < targetFrameTime) {
            float sleepTime = targetFrameTime - frameTime;
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
        }
    }


    // destroy_free_type();
    delete shader;
    delete confettiShader;
    delete texture;
    delete textureTarget;
    delete texturePostProcess;
    DestroyImgui();

    return 0;
}
