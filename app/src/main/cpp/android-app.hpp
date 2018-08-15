//
// Created by ekem on 8/11/2018.
//

#ifndef ANDROID_APP_HPP
#define ANDROID_APP_HPP

#include <jni.h>
#include <android/log.h>

#include <GLES3/gl3.h>

#define LOG_TAG   "test_default"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)


extern bool checkGlError(const char* functionName);
extern GLuint createShader(GLenum shaderType, const char* src);
extern GLuint createProgram(const char* VertexSrc, const char* fragmentSrc);

#endif //ANDROID_APP_HPP
