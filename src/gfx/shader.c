#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "gl_error.h"
#include "shader.h"

static char *load_file(const char *path)
{
    FILE *file = fopen(path, "rb"); // binary mode to prevent problems with crlf

    if (!file)
    {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = '\0';

    fclose(file);

    return data;
}

static GLuint compile_shader(char *source, GLuint type)
{
    GLuint id;
    GL_CALL(id = glCreateShader(type));
    GL_CALL(glShaderSource(id, 1, (const char *const *)&source,
                           NULL)); // null means the string is null terminated
    GL_CALL(glCompileShader(id));

    GLint result;
    GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

    if (result == GL_FALSE)
    {
        GLint length;
        GL_CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

        char message[length];
        GL_CALL(glGetShaderInfoLog(id, length, &length, message));

        printf("Failed to compile %s shader: %s\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment",
               message);
        GL_CALL(glDeleteShader(id));
        return 0;
    }

    return id;
}

GLuint shader_new(const char *vertex_shader_path, const char *frag_shader_path)
{
    GLuint program = glCreateProgram();

    char *vertex_shader = load_file(vertex_shader_path);
    char *fragment_shader = load_file(frag_shader_path);

#if __EMSCRIPTEN__
    // replace version directive
    char *version = "#version 300 es\n";

    int first_line_len = strchr(vertex_shader, '\n') - vertex_shader + 1;

    char *versioned_vertex_shader =
        (char *)malloc(strlen(version) + strlen(vertex_shader) - first_line_len + 1);
    strcpy(versioned_vertex_shader, version);
    strcat(versioned_vertex_shader, vertex_shader + first_line_len);
    free(vertex_shader);
    vertex_shader = versioned_vertex_shader;

    char *precision_specifier = "precision mediump float;\n";

    first_line_len = strchr(fragment_shader, '\n') - fragment_shader + 1;
    char *versioned_fragment_shader = (char *)malloc(strlen(version) + strlen(precision_specifier) +
                                                     strlen(fragment_shader) - first_line_len + 1);
    strcpy(versioned_fragment_shader, version);
    strcat(versioned_fragment_shader, precision_specifier);
    strcat(versioned_fragment_shader, fragment_shader + first_line_len);
    free(fragment_shader);
    fragment_shader = versioned_fragment_shader;
#endif

    GLuint vs = compile_shader(vertex_shader, GL_VERTEX_SHADER);
    GLuint fs = compile_shader(fragment_shader, GL_FRAGMENT_SHADER);

    GL_CALL(glAttachShader(program, vs));
    GL_CALL(glAttachShader(program, fs));
    GL_CALL(glLinkProgram(program));
    GL_CALL(glValidateProgram(program));

    // delete once linked
    GL_CALL(glDeleteShader(vs));
    GL_CALL(glDeleteShader(fs));

    return program;
}

void shader_use(GLuint shader) { GL_CALL(glUseProgram(shader)); }

void shader_delete(GLuint shader) { GL_CALL(glDeleteProgram(shader)); }

void shader_uniform_int(GLuint shader, const char *name, int value)
{
    GLint loc;
    GL_CALL(loc = glGetUniformLocation(shader, name));
    GL_CALL(glUniform1i(loc, value));
}

void shader_uniform_vec3f(GLuint shader, const char *name, Vec3f vec)
{
    GLint loc;
    GL_CALL(loc = glGetUniformLocation(shader, name));
    GL_CALL(glUniform3f(loc, vec.x, vec.y, vec.z));
}

void shader_uniform_vec4f(GLuint shader, const char *name, Vec4f vec)
{
    GLint loc;
    GL_CALL(loc = glGetUniformLocation(shader, name));
    GL_CALL(glUniform4f(loc, vec.x, vec.y, vec.z, vec.w));
}