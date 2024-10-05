#if !defined(GL_ERROR_H)
#define GL_ERROR_H

#include <stdbool.h>

#ifndef __EMSCRIPTEN__
// x86 only
#define ASSERT_BREAKPOINT(x)                                                                                 \
    if (!(x))                                                                                                \
    {                                                                                                        \
        asm("int $3");                                                                                       \
        asm("nop");                                                                                          \
    }

#define GL_CALL(x)                                                                                           \
    glClearError();                                                                                          \
    x;                                                                                                       \
    ASSERT_BREAKPOINT(glCheckError(#x, __FILE__, __LINE__))
#else
#define GL_CALL(x) x
#endif

void glClearError();
bool glCheckError(const char *function_name, const char *file, int line);

#endif // GL_ERROR_H
