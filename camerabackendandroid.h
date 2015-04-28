#ifndef CAMERABACKENDANDROID_H
#define CAMERABACKENDANDROID_H

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QOpenGLFunctions>
#include "camerabackend.h"
/**
 * @brief The CameraBackendAndroid class provides and access to cameras frames with GL_TEXTURE_EXTERNAL_OES.
 */
class CameraBackendAndroid: public CameraBackend
{
    Q_OBJECT
public:
    CameraBackendAndroid();
    ~CameraBackendAndroid();
    /**
     * @brief open Opens the camera. This methos has to be called from thread with active current OpenGL context.
     * @param width
     * @param height
     * @param textureId The OpenGL texture id, the texture must have a type GL_TEXTURE_EXTERNAL_OES. The texture must be associated to current context.
     * @return
     */
    bool open(int width, int height, GLuint textureId) override;
    void close() override;
    void updateFrame() override;
    GLenum textureType() override;

private:

    static void onFrameAvailable(JNIEnv *, jobject);

    QAndroidJniObject * java_backend;
    static CameraBackendAndroid * self;
};

#endif // CAMERABACKENDANDROID_H
