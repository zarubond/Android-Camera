#ifndef CAMERABACKEND_H
#define CAMERABACKEND_H

#include <QObject>
#include <qopengl.h>

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif

class CameraBackend : public QObject
{
    Q_OBJECT
public:
    explicit CameraBackend(QObject *parent = 0);
    ~CameraBackend();
    /**
     * @brief open Opens the camera. This has to be called with current OpenGL context
     * @param width Required width of the camera stream.
     * @param height Required height of the camera stream.
     * @param textureId OpenGL ID of the 2D texture with GL_RGBA colors and GL_UNSIGNED_BYTE pixel size.
     * @return True if the opening was successfull or false of not.
     */
    virtual bool open(int width, int height, GLuint textureId)=0;
    /**
     * @brief close Close the camera. This will stop updating the images from camera.
     */
    virtual void close()=0;
    /**
     * @brief updateFrame Pulls the image from the camera and uploads it to texture provided on start. This method must be called with current OpenGL context.
     */
    virtual void updateFrame()=0;

    /**
     * @brief textureType Returns the texture type required for the target textureId in start method.
     * @return
     */
    virtual GLenum textureType()=0;

signals:
    /**
     * @brief frameAvailable Trigers whenever new frame is available from camera.
     */
    void frameAvailable();

public slots:
};

#endif // CAMERABACKEND_H
