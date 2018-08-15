//
// Created by ekem on 7/29/2018.
//



#include <iostream>
#include <string>

#include "coin.hpp"

#include "android-app.hpp"
#include "renderer.hpp"


static void printGlString(const char* name, GLenum s)
{
    const char* v = (const char*)glGetString(s);
    LOGV("GL %s: %s", name, v);
}

bool checkGlError (const char* functionName)
{
    GLint err = glGetError();

    if (err != GL_NO_ERROR)
    {
        LOGE("GL error after %s(): 0x%08x\n", functionName, err);
        return true;
    }

    return false;
}

GLuint createShader(GLenum shaderType, const char* src)
{
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGlError("glCreateShader");
        return 0;
    }

    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0)
        {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog)
            {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                LOGI("%s", infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint createProgram(const char* vertexSrc, const char* fragmentSrc)
{
    GLuint program = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLint linked = GL_FALSE;


    // Compile vertex shader
    vertexShader = createShader(GL_VERTEX_SHADER, vertexSrc);
    if(!vertexShader)
        goto exit;

    // Compile fragment shader
    fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if(!fragmentShader)
        goto exit;

    program = glCreateProgram();
    if (!program)
    {
        checkGlError("glCreateProgram");
        goto exit;
    }

    // Attach shader
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    // Link program
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        LOGE("Could not link program %d", infoLogLen);
        if (infoLogLen)
        {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog)
            {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                LOGE("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

exit:
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

float p = .25;
Coin coin{};

void render()
{
    if (p == .25)
        glClearColor(1.f, .25f, 1.f, 1.f);
    else
        glClearColor(p, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void init()
{
}

void roll()
{
    p = coin.flip();
}

static Renderer* g_renderer = NULL;

extern "C" {
    JNIEXPORT void JNICALL
        Java_industries_hundred_ekem_quoin_MainGLSurfaceView_nativeRenderer(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL
        Java_industries_hundred_ekem_quoin_MainGLSurfaceView_init(JNIEnv * env, jobject obj);
    //JNIEXPORT void JNICALL
    //    Java_industries_hundred_ekem_test_1default_MainGLSurfaceView_resize(JNIEnv * env, jobject obj, int width, int height);
    JNIEXPORT void JNICALL
        Java_industries_hundred_ekem_quoin_MainGLSurfaceView_handleTouch(JNIEnv * env, jobject obj);
}

JNIEXPORT void JNICALL
Java_industries_hundred_ekem_quoin_MainGLSurfaceView_nativeRenderer(JNIEnv * env, jobject obj) {
    if (g_renderer)
        g_renderer->render();
}

JNIEXPORT void JNICALL
Java_industries_hundred_ekem_quoin_MainGLSurfaceView_init(JNIEnv * env, jobject obj) {
    printGlString("VERSION", GL_VERSION);
    printGlString("VENDOR", GL_VENDOR);
    printGlString("RENDERER", GL_RENDERER);
    printGlString("EXTENSIONS", GL_EXTENSIONS);

    g_renderer = new Renderer();
    g_renderer->init();
}

JNIEXPORT void JNICALL
Java_industries_hundred_ekem_quoin_MainGLSurfaceView_handleTouch(JNIEnv *env, jobject obj) {
    roll();
}

