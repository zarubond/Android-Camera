#include "camera.h"

#ifdef Q_OS_ANDROID

    #define CAMERA_EXTERNAL_OES
   // #define CAMERA_SUBTEX

#ifdef CAMERA_EXTERNAL_OES
    #include "camerabackendandroid.h"
    #include "camera2backendandroid.h"
#elif defined(CAMERA_SUBTEX)
    #include "camerabackendandroidsw.h"
#endif

#else
    #include "camerabackendqt.h"
#endif

Camera::Camera()
{
#ifdef Q_OS_ANDROID
    #ifdef CAMERA_EXTERNAL_OES
        backend=new CameraBackendAndroid();
    #elif defined(CAMERA_SUBTEX)
        backend=new CameraBackendAndroidSW();
    #endif
        //backend=new Camera2BackendAndroid();
#else
    backend=new CameraBackendQt();
#endif
    connect(backend, SIGNAL(frameAvailable(void)), this, SIGNAL(frameAvailable(void)), Qt::DirectConnection);
}

Camera::~Camera()
{

}

void Camera::start(int width, int height, GLuint textureId)
{
    backend->open(width, height, textureId);
}

void Camera::updateFrame()
{
    backend->updateFrame();
}

void Camera::stop()
{
    backend->close();
}

GLenum Camera::textureType()
{
    return backend->textureType();
}
