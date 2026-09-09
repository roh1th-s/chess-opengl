#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "window.h"

static void glfw_error_callback(int code, const char *description)
{
    fprintf(stderr, "GLFW error (%d): %s\n", code, description);
}

int window_init(Window *self, int width, int height, const char *title)
{
    const char *platform = getenv("CHESS_GLFW_PLATFORM");

    glfwSetErrorCallback(glfw_error_callback);

    if (platform != NULL && strcmp(platform, "wayland") == 0 &&
        glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
        // Only use wayland if explicitly requested
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    } else if (glfwPlatformSupported(GLFW_PLATFORM_X11)) {
        // GLEW uses GLX on Linux, so X11/XWayland is the compatible default.
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    }

    if (!glfwInit())
    {
        fprintf(stderr, "Error initializing GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // for MSAA
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!glfw_window)
    {
        fprintf(stderr, "Error creating window!\n");
        glfwTerminate();
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
