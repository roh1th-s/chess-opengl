#if !defined WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

typedef struct
{
    int width, height;
    const char *title;
    GLFWwindow *glfw_window;
} Window;

int window_init(Window *self, int width, int height, const char *title);
void window_swap_buffers(Window *self);
void window_poll_events();
int window_should_close(Window *self);
void window_destroy(Window *self);

#endif