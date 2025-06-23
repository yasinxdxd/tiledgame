#ifndef YT2D_WINDOW_HPP
#define YT2D_WINDOW_HPP
#include <Definitions.hh>
#include <string>
#ifndef _glfw3_h_
struct GLFWwindow;
#endif
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
#include <SDL.h>
#endif

// TODO: fix this structure via delete this global shit!

namespace ymath
{
    template <size_t _D, typename _Type>
    struct Vector;
}

namespace yt2d
{
    enum WindowFlag : unsigned int
    {
        WINDOW_TRANSPARENT = 0x01,
        WINDOW_NO_MENUBAR = 0x02,
        WINDOW_ALWAYS_FOCUS = 0x04,
    };

    YT2D_CORE_CLASS Window
    {
    public:
        explicit Window(const std::string& title, u32 width, u32 height, int flags = 0);
        explicit Window(const char* title, u32 width, u32 height, int flags = 0);
        explicit Window(const std::string& title, ymath::Vector<2, float> size, int flags = 0);
        explicit Window(const char* title, ymath::Vector<2, float> size, int flags = 0);
        ~Window();
    
    public:
#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
        operator SDL_Window*()
        {
            return m_sdl_window;
        }
#else
        operator GLFWwindow*()
        {
            return m_window;
        }
#endif

        void pollEvent(void);
        void waitEvent(void);
        YT2D_NODISCARD bool isClose(void);
        void clear(void);
        void clear(float r, float g, float b, float a = 1.f);
        void display(void);

        u32 getWindowWidth();
        u32 getWindowHeight();

        void setWindowPos(int x, int y);
        void setWindowPos(ymath::Vector<2, int> pos);

        void setCurrentContext();
        void detachCurrentContext();

        bool isWindowHovered();

    private:
        YT2D_NODISCARD YT2D_STATUS Init(int flags);

    private:
        u32 m_width;
        u32 m_height;
        std::string m_title;

#if defined __EMSCRIPTEN__ || defined SDL_BACKEND
    private:
        SDL_Window* m_sdl_window;
        SDL_Event m_sdl_event;
        SDL_Renderer* m_sdl_renderer;
        SDL_GLContext m_sdl_gl_context;
        bool m_sdl_is_close = false;
    public:
        SDL_GLContext& getSDLGLContext() { return m_sdl_gl_context; }
#else
    private:
        GLFWwindow* m_window;
#endif

    };
}



namespace _priv::callbacks
{
    static inline bool is_glfw_init = false;
    static inline bool is_glad_init = false;
    static inline uint16_t created_window_count = 0;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void glfw_error_callback(int error, const char* description);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void glfw_window_focus_callback(GLFWwindow* window, int focused);
    static void glfw_window_always_focus_callback(GLFWwindow* window, int focused);
#ifdef WIN32
#else
#define __stdcall
#endif
    static void __stdcall gl_debug_callback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam);
}

#endif//YT2D_WINDOW_HPP