#include <cstdint>


// type stuff
#define TYPEABBR(TYPE, ABBR, COUNT) using ABBR##COUNT = TYPE##COUNT##_t; \
    static_assert(sizeof(TYPE##COUNT##_t) == COUNT##uz/8uz, #TYPE " is not " #COUNT "-bit.")

TYPEABBR( int, i,  8);
TYPEABBR(uint, u,  8);
TYPEABBR( int, i, 16);
TYPEABBR(uint, u, 16);
TYPEABBR( int, i, 32);
TYPEABBR(uint, u, 32);
TYPEABBR( int, i, 64);
TYPEABBR(uint, u, 64);
using f32 = float;
using f64 = double;
using std::size_t;

#undef TYPEABBR
