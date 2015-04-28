#include "camerabackendandroid.h"

#include <QDebug>

CameraBackendAndroid::CameraBackendAndroid():
    java_backend(NULL)
{
    self=this;

    JNINativeMethod methods[] { {"frameAvailable", "()V", reinterpret_cast<void *>(CameraBackendAndroid::onFrameAvailable)}};

    if (QAndroidJniObject::isClassAvailable("com/zarubond/lookingglass/CameraBackend"))
    {
        java_backend=new QAndroidJniObject("com/zarubond/lookingglass/CameraBackend","()V");
        if(java_backend->isValid())
        {
            QAndroidJniEnvironment env;
            jclass objectClass = env->GetObjectClass(java_backend->object<jobject>());
            env->RegisterNatives(objectClass, methods,  sizeof(methods) / sizeof(methods[0]));
            env->DeleteLocalRef(objectClass);
        }
    }
}

CameraBackendAndroid::~CameraBackendAndroid()
{
    close();
    delete java_backend;
}

bool CameraBackendAndroid::open(int width, int height, GLuint textureId)
{
    java_backend->callMethod<void>("startPreview", "(III)V",(jint)width, (jint)height, (jint) textureId);
    return true;
}

void CameraBackendAndroid::close()
{
    java_backend->callMethod<void>("stopPreview", "()V");
}

void CameraBackendAndroid::updateFrame()
{
    java_backend->callMethod<void>("updateFrame", "()V");
}

GLenum CameraBackendAndroid::textureType()
{
    return GL_TEXTURE_EXTERNAL_OES;
}

void CameraBackendAndroid::onFrameAvailable(JNIEnv *, jobject)
{
    emit self->frameAvailable();
}

CameraBackendAndroid * CameraBackendAndroid::self=NULL;
