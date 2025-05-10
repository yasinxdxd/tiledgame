#ifndef CAM_HPP
#define CAM_HPP

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <input.hpp>
#include <imgui.h>
#define SHAPE_QUAD
#define SHAPE_CUBE
#include <renderables.hpp>

extern float g_scroll_delta;

struct Camera
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    float distance = 50.0f;

    glm::mat4 view;
    glm::mat4 projection;

    Camera()
    {
        projection = glm::perspective(glm::radians(60.f), 16.0f / 9.0f, 0.1f, 10000.f);
        updateView();
    }

    void move(yt2d::Window& window) {
        // Orbit with right-click drag
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            if (ImGui::GetMousePos().x > window.getWindowWidth() / 2) {
                ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
    
                float sensitivity = 0.005f;
                yaw   += delta.x * sensitivity;
                pitch += delta.y * sensitivity;
                // Clamp pitch to avoid flipping
                pitch = glm::clamp(pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
            }
        }
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            if (ImGui::GetMousePos().x > window.getWindowWidth() / 2) {
                ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    
                float sensitivity = 0.005f;
                yaw   += delta.x * sensitivity;
                pitch += delta.y * sensitivity;
                // Clamp pitch to avoid flipping
                pitch = glm::clamp(pitch, -glm::half_pi<float>() + 0.01f, glm::half_pi<float>() - 0.01f);
            }
        }

        if (ImGui::GetMousePos().x > window.getWindowWidth() / 2) {
            if (g_scroll_delta != 0.0f) {
                distance -= g_scroll_delta * distance * 0.1f;
                distance = glm::clamp(distance, 0.1f, 10000.0f);
                g_scroll_delta = 0.0f; // reset after consuming
            }
        }

        updateView();
    }

    void updateView() {
        // Spherical to Cartesian
        glm::vec3 camOffset;
        camOffset.x = distance * cosf(pitch) * sinf(yaw);
        camOffset.y = distance * sinf(pitch);
        camOffset.z = distance * cosf(pitch) * cosf(yaw);

        position = camOffset;
        view = glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
    }
};


struct GameObj {

    GameObj() {
        model = glm::mat4(1.0);
        // cube.load_mesh_obj("cube.obj", vertices_size, indices_size);
    }

    Cube cube;
    unsigned int color = 0xFFFFFFFF;
    glm::mat4 model;
    glm::vec3 pos;
    // size_t vertices_size;
    // size_t indices_size;

    void setPos(glm::vec3 pos) {
        this->pos = pos;
        model = glm::mat4(1.0);
        model = glm::translate(model, this->pos);
    }

    void draw() {

    }

    bool operator==(const GameObj& other) const {
        return color == other.color; // compare what matters
    }
};

#endif//CAM_HPP