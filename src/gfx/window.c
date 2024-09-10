#include <GLFW/glfw3.h>
#include <stdio.h>

#include "window.h"

int window_init(Window *self, int width, int height, const char *title)
{
    if (!glfwInit())
    {
        printf("Error!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef __EMSCRIPTEN__
    glfwWindowHint(GLFW_SAMPLES, 4); // for MSAA
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#endif

    GLFWwindow *glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!glfw_window)
    {
        printf("Error creating window!\n");
        return -1;
    }

    self->glfw_window = glfw_window;
    self->height = height;
    self->width = width;
    self->title = title;

    glfwMakeContextCurrent(glfw_window);
    glfwSwapInterval(1); // enable vsync

    return 0;
}

void window_poll_events() { glfwPollEvents(); }

void window_swap_buffers(Window *self) { glfwSwapBuffers(self->glfw_window); }

int window_should_close(Window *self) { return glfwWindowShouldClose(self->glfw_window); }

void window_destroy(Window *self)
{
    glfwDestroyWindow(self->glfw_window);
    glfwTerminate();
}
