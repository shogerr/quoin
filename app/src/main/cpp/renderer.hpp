//
// Created by ekem on 8/11/2018.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "android-app.hpp"
#include <GLES3/gl3.h>

#include <math.h>

#define STR(s) #s
#define STRV(s) STR(s)

#define TWO_PI (2.0 * M_PI)

class Renderer {
public:
    Renderer();
    bool init();
    void render();

private:
    enum {VB_SCALEROT, VB_COLOR, VB_INSTANCE, VB_COUNT};
    void step();
    void draw();

    uint64_t mLastFrameNs;
    GLuint mProgram;
    GLuint mVB[VB_COUNT];
    GLuint mVA;

    float mTheta = 0;
};

#endif //RENDERER_HPP
