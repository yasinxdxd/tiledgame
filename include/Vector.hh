#ifndef YT2D_VECTOR_HH
#define YT2D_VECTOR_HH

namespace ymath
{
    template <size_t _D, typename _Type>
    struct Vector
    {
        _Type x, y;
    };

    typedef Vector<2, float> Vec2f;
}

#endif