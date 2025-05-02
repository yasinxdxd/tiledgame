#ifndef YT2D_DEFINITIONS_HH
#define YT2D_DEFINITIONS_HH

// stl includes
#ifndef YT2D_NO_STDINT
#include <stdint.h>
#else
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
#endif

//YT2D_API pre definition is going to use with core game api.
#ifndef YT2D_NO_EXTERN
#define YT2D_API extern
#else
#define YT2D_API
#endif
//core classes of engine
#define YT2D_CORE_CLASS class
#if __cplusplus >= 201703L
#define YT2D_NODISCARD [[nodiscard]]
#else
#define YT2D_NODISCARD
#endif

// type definitions (using) for integrals
// signed definitions
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

//ERROR CODES
#define YT2D_STATUS i16
#define YT2D_STATUS_ERROR -1
#define YT2D_STATUS_SUCCES 0

// unsigned definitions
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

//memory defines
#include <memory>

template<typename _Ptr>
using Unique = std::unique_ptr<_Ptr>;
template<typename _Ptr>
using Shared = std::shared_ptr<_Ptr>;
template<typename _Ptr>
using Weak = std::weak_ptr<_Ptr>;

#endif//YT2D_DEFINITIONS_HH