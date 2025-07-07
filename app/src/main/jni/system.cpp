// File: app/src/main/jni/system.cpp
#define LOG_TAG "SocksVpnPlugin"

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/un.h>
#include <sys/socket.h>
#include <ancillary.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// JNI wrapper for static native System.jniclose(int fd)
extern "C"
JNIEXPORT void JNICALL
Java_com_jforu_proxies_System_jniclose(JNIEnv *env, jclass clazz, jint fd) {
    close(fd);
}

// JNI wrapper for static native System.sendfd(int tun_fd, String sockPath)
extern "C"
JNIEXPORT jint JNICALL
Java_com_jforu_proxies_System_sendfd(JNIEnv *env, jclass clazz, jint tun_fd, jstring sock) {
    int fd;
    struct sockaddr_un addr;
    const char *sockpath = env->GetStringUTFChars(sock, nullptr);

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        LOGE("socket() failed: %s (fd=%d)", strerror(errno), fd);
        env->ReleaseStringUTFChars(sock, sockpath);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);
    env->ReleaseStringUTFChars(sock, sockpath);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        LOGE("connect() failed: %s (fd=%d)", strerror(errno), fd);
        close(fd);
        return -1;
    }

    if (ancil_send_fd(fd, tun_fd)) {
        LOGE("ancil_send_fd() failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// المسار الكامل للكلاس في الجافا
static const char *classPathName = "com/jforu/proxies/System";

// ربط أسماء الدوال بالـ JNI
static JNINativeMethod method_table[] = {
    { "jniclose", "(I)V",
        (void*)Java_com_jforu_proxies_System_jniclose },
    { "sendfd", "(ILjava/lang/String;)I",
        (void*)Java_com_jforu_proxies_System_sendfd }
};

static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* methods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (!clazz) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerAll(JNIEnv* env) {
    return registerNativeMethods(env, classPathName,
                                 method_table,
                                 sizeof(method_table)/sizeof(method_table[0]))
           ? JNI_TRUE : JNI_FALSE;
}

jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_4) != JNI_OK) {
        LOGE("GetEnv failed");
        return -1;
    }
    if (!registerAll(env)) {
        LOGE("registerAll failed");
        return -1;
    }
    LOGI("JNI_OnLoad succeeded");
    return JNI_VERSION_1_4;
}
