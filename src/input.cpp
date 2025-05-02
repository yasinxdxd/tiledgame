#include <input.hpp>

#define MOUSE_POS_NOW 0
#define MOUSE_POS_PREV 1

bool Input::isKeyReleased(int key)
{
    m_key = key;
    if (r_is_key_released)
    {
        r_is_key_released = false;
        return true;
    }
    return false;
}

bool Input::isKeyPressed(int key)
{
    m_key = key;
    if (r_is_key_pressed)
    {
        r_is_key_pressed = false;
        return true;
    }
    return false;
}

bool Input::isMouseReleased(int button)
{
    m_button = button;
    if (r_is_button_released)
    {
        r_is_button_released = false;
        return true;
    }
    return false;
}

bool Input::isMousePressed(int button)
{
    m_button = button; // BIG FIXME: use buttons array to evaluate all button types, because one function changes the end button type for the main engine loop!!!
    if (r_is_button_pressed)
    {
        //r_is_button_pressed = false;
        return true;
    }
    return false;
}

void Input::getMousePosition(double &x, double &y)
{
    x = Input::m_xposes[MOUSE_POS_NOW];
    y = Input::m_yposes[MOUSE_POS_NOW];
}

void Input::getPrevMousePosition(double &x, double &y)
{
    x = Input::m_xposes[MOUSE_POS_PREV];
    y = Input::m_yposes[MOUSE_POS_PREV];
}
