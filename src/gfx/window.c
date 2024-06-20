#include <GLFW/glfw3.h>
#include <stdio.h>

#include "window.h"

int window_init(Window *w, int width, int height, const char *title)
{
    if (!glfwInit())
    {
        printf("Error!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!glfwWindow)
    {
        printf("Error creating window!\n");
        return -1;
    }

    w->glfwWindow = glfwWindow;
    w->height = height;
    w->width = width;
    w->title = title;

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1); // enable vsync

    return 0;
}

void window_update(Window *w)
{
    glfwSwapBuffers(w->glfwWindow);
    glfwPollEvents();
}

int window_should_close(Window *w)
{
    return glfwWindowShouldClose(w->glfwWindow);
}

void window_destroy(Window *w)
{
    glfwDestroyWindow(w->glfwWindow);
    glfwTerminate();
}
