#ifndef CAMERA_H
#define CAMERA_H

#include <QtCore>
#include "camerabackend.h"

class Camera: public QObject
{
    Q_OBJECT
public:
    Camera();
    ~Camera();
    /**
     * @brief start Start camera.
     * @param width Width of the camera view and must be identical to the width of textureId.
     * @param height Width of the camera view and must be identical to the width of textureId.
     * @param textureId OpenGL texture id. The size of the texture must be identical to width and height.
     */
    void start(int width, int height, GLuint textureId);
    /**
     * @brief requestFrame Load a new frame to texture provided on start. I will do nothing if there is no new frame. Before calling this function
     * you must bind context where the OpenGL texture lives.
     */
    void updateFrame();
    /**
     * @brief stop Stop camera and all operations.
     */
    void stop();
    /**
     * @brief textureType Returns the texture type required for the target textureId in start method.
     * @return
     */
    GLenum textureType();

signals:
    /**
     * @brief frameAvailable Emited whenever new frame is available.
     */
    void frameAvailable();
private:
    CameraBackend * backend;
};

#endif // CAMERA_H
