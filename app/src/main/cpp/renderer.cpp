//
// Created by ekem on 8/11/2018.
//

#include "renderer.hpp"
#include <time.h>

#define SCALEROT_ATTRIB 0
#define COLOR_ATTRIB 1
#define POS_ATTRIB 2

struct Vertex {
    GLfloat pos[4];
};

Vertex triangle[6] = {{0.0, -0.5, 0.0, 1.0},
                      {0.5, -0.25, 0.0, 1.0},
                      {-0.5, -.25, 0.0, 1.0},
                      {0.5, 0.25, 0.0, 1.0},
                      {-0.5, 0.25, 0.0, 1.0},
                      {0.0, 0.5, 0.0, 1.0}};

static const GLchar VERTEX_SHADER[] =
        "#version 300 es\n"
        "uniform mat4 projMatrix;\n"
        "layout(location=" STRV(POS_ATTRIB) ") in vec4 pos;\n"
        "layout(location=" STRV(COLOR_ATTRIB) ") in vec4 color;\n"
        "layout(location=" STRV(SCALEROT_ATTRIB) ") in vec4 scaleRot;\n"
        "//layout(location=" STRV(OFFSET_ATTRIB) ") in vec2 offset;\n"
        "out vec4 vColor;\n"
        "const vec4 vertices[3] = vec4[3](vec4(0.5, -0.5, 0.5, 1.0),\n"
        "                                 vec4(-0.5, -0.5, 0.5, 1.0),\n"
        "                                 vec4(0.5, 0.5, 0.5, 1.0));\n"
        "void main() {\n"
        "    mat4 sr_x = mat4(vec4(1.0, 0.0, 0.0, 0.0), vec4(0.0, scaleRot.xy, 0.0), vec4(0.0, scaleRot.zw, 0.0), vec4(0.0, 0.0, 0.0, 1.0));\n"
        "    mat4 sr_y = mat4(vec4(scaleRot.x, 0.0, scaleRot.z, 0.0), vec4(0.0, 1.0, 0.0, 0.0),  vec4(scaleRot.y, 0.0, scaleRot.w, 0.0), vec4(0.0, 0.0, 0.0, 1.0));\n"
        "    mat4 sr_z = mat4(vec4(scaleRot.xy, 0.0, 0.0), vec4(scaleRot.zw, 0.0, 0.0),  vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0));\n"
        "//    mat2 st = mat2(scaleRot.xy, scaleRot.zw);\n"
        "//    gl_Position = vec4(sr*pos + offset, 0.5, 1.0);\n"
        "//    vColor = color;\n"
        "    gl_Position = sr_z * sr_y * pos;\n"
        "    vColor = color;\n"
        "//      gl_Position = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n";

static const GLchar FRAGMENT_SHADER[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec4 vColor;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "    //outColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
        "    outColor = vColor;\n"
        "    //outColor = vec4(0.500460, 0.865760, -0.865760, 0.500460);\n"
        "}\n";

Renderer::Renderer()
{
    mLastFrameNs = 0;
}

void Renderer::render()
{
    step();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    checkGlError("Renderer::render");
}

void Renderer::step()
{
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    auto nowNs = now.tv_sec*1000000000ull + now.tv_nsec;

    if (mLastFrameNs > 0)
    {
        float dt = float(nowNs - mLastFrameNs) * .000000001f;

        glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
        float* transforms = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 4*sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        mTheta += dt*2;

        if (mTheta >= TWO_PI)
            mTheta -= TWO_PI;
        else if (mTheta <= -TWO_PI)
            mTheta += TWO_PI;

        float s = sinf(mTheta);
        float c = cosf(mTheta);
        transforms[0] = c;
        transforms[1] = s;
        transforms[2] = -s;
        transforms[3] = c;

        glUnmapBuffer(GL_ARRAY_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_COLOR]);
        float* colors = (float*)glMapBufferRange(GL_ARRAY_BUFFER,0, 4*sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        colors[0] = 1.0;
        colors[1] = 1.0;
        colors[2] = 0.0;
        colors[3] = 1.0;


        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    mLastFrameNs = nowNs;
}

void Renderer::draw()
{
    glUseProgram(mProgram);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, 1);
}

bool Renderer::init()
{
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgram)
        return false;

    LOGI("SCALEROT_ATTRIB: %d VB_SCALEROT: %d sizeof(float): %d, VB_COUNT: %d, GL_FLOAT: %d", SCALEROT_ATTRIB, VB_SCALEROT, (uint)sizeof(float), VB_COUNT, GL_FLOAT);
    glGenBuffers(VB_COUNT, mVB);

    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), &triangle[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_COLOR]);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &mVA);
    glBindVertexArray(mVA);

    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
    glVertexAttribPointer(POS_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(POS_ATTRIB);

    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
    glVertexAttribPointer(SCALEROT_ATTRIB, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(SCALEROT_ATTRIB);
    glVertexAttribDivisor(SCALEROT_ATTRIB, 1);

    glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_COLOR]);
    glVertexAttribPointer(COLOR_ATTRIB, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(COLOR_ATTRIB);
    glVertexAttribDivisor(COLOR_ATTRIB, 1);
    return true;
}
