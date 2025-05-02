#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui.h> // Ensure ImGui is included for GetIO()
#include <imgui_impl_glfw.h>
#include <Window.hpp>
#include <Vector.hh>
#include <input.hpp>
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <imgui_impl_sdl.h>
#endif
#include <iostream>


yt2d::Window::Window(const std::string& title, u32 width, u32 height, int flags):
    m_title(title), m_width(width), m_height(height)
{
    auto a = Init(flags);
}

yt2d::Window::Window(const char* title, u32 width, u32 height, int flags):
    m_title(title), m_width(width), m_height(height)
{
    auto a = Init(flags);
}

yt2d::Window::Window(const std::string& title, ymath::Vec2f size, int flags):
    m_title(title), m_width(size.x), m_height(size.y)
{
    auto a = Init(flags);
}

yt2d::Window::Window(const char* title, ymath::Vec2f size, int flags):
    m_title(title), m_width(size.x), m_height(size.y)
{
    auto a = Init(flags);
}

yt2d::Window::~Window()
{
    
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    SDL_GL_DeleteContext(m_sdl_gl_context);
    SDL_DestroyWindow(m_sdl_window);
    SDL_DestroyRenderer(m_sdl_renderer);
	SDL_Quit();
#else
	glfwDestroyWindow(m_window);
    _priv::callbacks::created_window_count--;
    // FIXME: count the total windows created and just after there is no window any more terminate the glfw!
    if (_priv::callbacks::created_window_count == 0) {
        glfwTerminate();
    }
#endif
}

void yt2d::Window::pollEvent(void)
{
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    while (SDL_PollEvent(&m_sdl_event))
	{
		// without it you won't have keyboard input and other things
		ImGui_ImplSDL2_ProcessEvent(&m_sdl_event);
		// you might also want to check io.WantCaptureMouse and io.WantCaptureKeyboard
		// before processing events

		switch (m_sdl_event.type)
		{
		case SDL_QUIT:
			m_sdl_is_close = true;
			break;

		case SDL_WINDOWEVENT:
			switch (m_sdl_event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				
				break;
			}
			break;

		case SDL_KEYDOWN:
			switch (m_sdl_event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_sdl_is_close = true;
				break;
			}
			break;
		}
	}
#else
    return glfwPollEvents();
#endif
}

void yt2d::Window::waitEvent(void)
{
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    /*...*/
#else
    return glfwWaitEvents();
#endif
}

YT2D_NODISCARD bool yt2d::Window::isClose(void)
{
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    return m_sdl_is_close;
#else
    return glfwWindowShouldClose(this->m_window);
#endif
}

void yt2d::Window::clear(void)
{
    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //glClearColor(0.9, 0.9, 0.9, 1);
}

void yt2d::Window::clear(float r, float g, float b, float a)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//glClear(GL_COLOR_BUFFER_BIT);
    //glClearColor(0.9, 0.9, 0.9, 1);
    glClearColor(r, g, b, a);
}

void yt2d::Window::display(void)
{
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    SDL_GL_SwapWindow(m_sdl_window);
#else
    glfwSwapBuffers(m_window);
#endif
}

static int get_bit(int* data, int bit)
{
    // assert(bit >= 0 && bit < 8);
    int setter = 1 << bit;
    int result = (*data & setter) >> bit;
    return result;
}

YT2D_NODISCARD YT2D_STATUS yt2d::Window::Init(int flags)
{
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    SDL_Init(SDL_INIT_VIDEO);
    
	SDL_CreateWindowAndRenderer(m_width, m_height, SDL_WINDOW_OPENGL, &m_sdl_window, &m_sdl_renderer);
	
    SDL_SetWindowTitle(m_sdl_window, m_title.c_str());

	// imgui and gl thing
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	SDL_version compiled;
    SDL_VERSION(&compiled);

    SDL_version linked;
    SDL_GetVersion(&linked);

	//gl context
	m_sdl_gl_context = SDL_GL_CreateContext(m_sdl_window);
    
    if (m_sdl_gl_context == NULL)
    {
        SDL_GL_DeleteContext(m_sdl_gl_context);
        SDL_DestroyWindow(m_sdl_window);
	    SDL_DestroyRenderer(m_sdl_renderer);
        return YT2D_STATUS_ERROR;
    }
    SDL_GL_MakeCurrent(m_sdl_window, m_sdl_gl_context);
    
    // enable VSync
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        return YT2D_STATUS_ERROR;
    }
    else
        std::cout << "[INFO] glad initialized" << std::endl;
#else
    //Init GLFW Library
    if(_priv::callbacks::is_glfw_init == false)
    {
        if (!glfwInit()) {
            glfwTerminate();
            return YT2D_STATUS_ERROR;
        }
        _priv::callbacks::is_glfw_init = true;
    }

    //Window Hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);// Disable Debug Context for now?!

    // IMPORTANT: initally not iconified for glfwFocusWindow to work
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    // flags
    if (get_bit(&flags, 0) == 1)
    {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    }
    if (get_bit(&flags, 1) == 1)
    {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

    //create the window
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

    //Check window creation
    if(m_window == nullptr)
    {
        //FIXME: check for other windows
        glfwTerminate();
        return YT2D_STATUS_ERROR;
    }

    glfwMakeContextCurrent(m_window);

    glfwSetFramebufferSizeCallback(m_window, _priv::callbacks::framebuffer_size_callback);
    glfwSetErrorCallback(_priv::callbacks::glfw_error_callback);
    glfwSetKeyCallback(m_window, _priv::callbacks::glfw_key_callback);
    glfwSetCursorPosCallback(m_window, _priv::callbacks::glfw_cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, _priv::callbacks::glfw_mouse_button_callback);

    if (get_bit(&flags, 2) == 1)
    {
        glfwSetWindowFocusCallback(m_window, _priv::callbacks::glfw_window_always_focus_callback);
    } else {
        glfwSetWindowFocusCallback(m_window, _priv::callbacks::glfw_window_focus_callback);
    }
    
    //GLAD Library Init
    if (_priv::callbacks::is_glad_init == false)
    {
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            return YT2D_STATUS_ERROR;
        }
        _priv::callbacks::is_glad_init = true;
    }
    // Add this function to handle OpenGL debug messages
    

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);  // Cull back faces
    // glFrontFace(GL_CCW);  // Counter-clockwise winding defines front faces

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "gl init error: " << error << std::endl;
    }

    // shitty way to check extension it just gives GL_INVALID_ENUM no more!!!
    // const GLubyte* extensions = glGetString(GL_EXTENSIONS);
    // if (extensions && strstr(reinterpret_cast<const char*>(extensions), "GL_ARB_debug_output"))
    // {
    //     // Enable the debug output
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glDebugMessageCallback(_priv::callbacks::gl_debug_callback, nullptr);
    // }
    // else
    // {
    //     // The extension is not supported; handle this case accordingly
    //     std::cout << "no extension" << std::endl;
    // }
#endif

    _priv::callbacks::created_window_count++;

    return YT2D_STATUS_SUCCES;

}
float g_scroll_delta = 0.0f;
void _priv::callbacks::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_scroll_delta += (float)yoffset;
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    // std::cout << "GLFW Scroll: x=" << xoffset << ", y=" << yoffset << std::endl;
    // if (camera)
    //     camera->onScroll(yoffset);
}

void _priv::callbacks::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void _priv::callbacks::glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
}

void GLAPIENTRY _priv::callbacks::gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "OpenGL Debug Message: " << message << std::endl;
}

// void _priv::callbacks::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//     if (key == Input::m_key && action == GLFW_RELEASE)
//     {
//         std::cout << "callback!" << std::endl;
//         Input::r_is_key_released = true;
//     }
//     if (key == Input::m_key && action == GLFW_PRESS)
//     {
//         Input::r_is_key_pressed = true;
//     }
// }

// void _priv::callbacks::glfw_cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
// {
//     Input::m_xposes[1] = Input::m_xpos;
//     Input::m_yposes[1] = Input::m_ypos;
//     Input::m_xpos = xpos;
//     Input::m_ypos = ypos;
//     Input::m_xposes[0] = Input::m_xpos;
//     Input::m_yposes[0] = Input::m_ypos;
// }

// void _priv::callbacks::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
// {
//     Input::r_is_button_released = false;
//     Input::r_is_button_pressed = false;
//     if (button == Input::m_button && action == GLFW_RELEASE)
//     {
//         Input::r_is_button_released = true;
//     }
//     if (button == Input::m_button && action == GLFW_PRESS)
//     {
//         Input::r_is_button_pressed = true;
//     }
// }

void _priv::callbacks::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) return; // Let ImGui handle the event
    if (key == Input::m_key && action == GLFW_RELEASE) {
        std::cout << "Key released!" << std::endl;
        Input::r_is_key_released = true;
    }
    if (key == Input::m_key && action == GLFW_PRESS) {
        Input::r_is_key_pressed = true;
    }
}

void _priv::callbacks::glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return; // Let ImGui handle the event
    Input::m_xposes[1] = Input::m_xpos;
    Input::m_yposes[1] = Input::m_ypos;
    Input::m_xpos = xpos;
    Input::m_ypos = ypos;
    Input::m_xposes[0] = Input::m_xpos;
    Input::m_yposes[0] = Input::m_ypos;
}

void _priv::callbacks::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return; // Let ImGui handle the event
    Input::r_is_button_released = false;
    Input::r_is_button_pressed = false;
    if (button == Input::m_button && action == GLFW_RELEASE) {
        Input::r_is_button_released = true;
    }
    if (button == Input::m_button && action == GLFW_PRESS) {
        Input::r_is_button_pressed = true;
    }
}

void _priv::callbacks::glfw_window_focus_callback(GLFWwindow *window, int focused)
{
    if (focused)
    {
        // The window gained input focus
    }
    else
    {
        // The window lost input focus
    }
}

void _priv::callbacks::glfw_window_always_focus_callback(GLFWwindow * window, int focused)
{
    if (focused)
    {
        // The window gained input focus
    }
    else
    {
        // The window lost input focus
        glfwRequestWindowAttention(window);
        glfwFocusWindow(window);
    }
}

u32 yt2d::Window::getWindowWidth()
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    m_width = width;
    return m_width;
}

u32 yt2d::Window::getWindowHeight()
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    m_height = height;
    return m_height;
}

void yt2d::Window::setWindowPos(int x, int y)
{
    glfwSetWindowPos(m_window, x, y);
}

void yt2d::Window::setWindowPos(ymath::Vector<2, int> pos)
{
    glfwSetWindowPos(m_window, pos.x, pos.y);
}

void yt2d::Window::setCurrentContext()
{
    glfwMakeContextCurrent(m_window);
}

void yt2d::Window::detachCurrentContext()
{
    glfwMakeContextCurrent(NULL);
}

bool yt2d::Window::isWindowHovered()
{
    if (glfwGetWindowAttrib(m_window, GLFW_HOVERED))
        return true;
        
    return false;
}
