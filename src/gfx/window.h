#if !defined WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

typedef struct {
    int width, height;
    const char* title;
    GLFWwindow* glfwWindow;
} Window;

int window_init(Window* self, int width, int height, const char* title);
void window_update(Window* self);
int window_should_close(Window* self);
void window_destroy(Window* self);

#endif