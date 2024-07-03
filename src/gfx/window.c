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
    glfwWindowHint(GLFW_SAMPLES, 4); // for MSAA

    GLFWwindow *glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!glfwWindow)
    {
        printf("Error creating window!\n");
        return -1;
    }

    self->glfwWindow = glfwWindow;
    self->height = height;
    self->width = width;
    self->title = title;

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1); // enable vsync

    return 0;
}

void window_update(Window *self)
{
    glfwSwapBuffers(self->glfwWindow);
    glfwPollEvents();
}

int window_should_close(Window *self)
{
    return glfwWindowShouldClose(self->glfwWindow);
}

void window_destroy(Window *self)
{
    glfwDestroyWindow(self->glfwWindow);
    glfwTerminate();
}
