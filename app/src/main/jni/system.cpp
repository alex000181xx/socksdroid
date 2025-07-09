// android/app/src/main/cpp/jforupro.cpp

#define LOG_TAG "jforupro"

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

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ✏️ لاحظ 'Jforupro' بحرف كبير
void Java_com_jforu_proxies_Jforupro_jniclose(JNIEnv *env, jclass cls, jint fd) {
    close(fd);
}

jint Java_com_jforu_proxies_Jforupro_sendfd(JNIEnv *env, jclass cls,
                                            jint tun_fd, jstring sock) {
    int fd;
    struct sockaddr_un addr;
    const char *sockpath = env->GetStringUTFChars(sock, 0);

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        LOGE("socket() failed: %s (socket fd = %d)", strerror(errno), fd);
        env->ReleaseStringUTFChars(sock, sockpath);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);
    env->ReleaseStringUTFChars(sock, sockpath);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        LOGE("connect() failed: %s (fd = %d)", strerror(errno), fd);
        close(fd);
        return -1;
    }

    if (ancil_send_fd(fd, tun_fd) != 0) {
        LOGE("ancil_send_fd: %s", strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static const char *classPathName = "com/jforu/proxies/Jforupro";

static JNINativeMethod method_table[] = {
    { "jniclose", "(I)V",
        (void*)Java_com_jforu_proxies_Jforupro_jniclose },
    { "sendfd", "(ILjava/lang/String;)I",
        (void*)Java_com_jforu_proxies_Jforupro_sendfd }
};

static int registerNativeMethods(JNIEnv* env) {
    jclass clazz = env->FindClass(classPathName);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", classPathName);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, method_table,
             sizeof(method_table)/sizeof(method_table[0])) < 0) {
        LOGE("RegisterNatives failed for '%s'", classPathName);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    if (!registerNativeMethods(env)) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
