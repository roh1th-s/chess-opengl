#if !defined(TYPES_H)
#define TYPES_H

typedef struct {
    int x;
    int y;
} Vec2i;

typedef struct {
    float x;
    float y;
} Vec2f;

typedef struct {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vec4f;

typedef struct {
    int r;
    int g;
    int b;
} Color3i;

typedef struct {
    float r;
    float g;
    float b;
} Color3f;

typedef struct {
    int r;
    int g;
    int b;
    int a;
} Color4i;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} Color4f;

#endif