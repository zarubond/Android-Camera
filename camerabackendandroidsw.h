#ifndef CAMERABACKENDANDROIDSW_H
#define CAMERABACKENDANDROIDSW_H

#include <QObject>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>

#include <QMutex>
#include <QThread>
#include <QElapsedTimer>

#define CAMERA_GL
#define CAMERA_SW

typedef unsigned int GLuint;

//http://stackoverflow.com/questions/23261662/how-to-use-graphicbuffer-in-android-ndk
//http://snorp.net/2011/12/16/android-direct-texture.html
//https://qt.gitorious.org/qtplayground/scenegraph/source/22095133d2af68f78d6905f215035f02a8933e68:customcontext/texture/eglgralloctexture.h

/**
 * @brief The CameraBackendAndroidSW class provides frames from camera and uploads them to opengl texture.
 */
class CameraBackendAndroidSW : public QObject
{
    Q_OBJECT
public:
    explicit CameraBackendAndroidSW(QObject *parent = 0);
    ~CameraBackendAndroidSW();

    void start(int width, int height, GLuint textureId);
    void stop();
    void updateFrame();

signals:
    void frameAvailable();

private:

    static void onFrameAvailable(JNIEnv *env, jobject, jbyteArray NV21FrameData, jint width, jint height);

    int width, height;
    QAndroidJniObject * java_backend;
    static CameraBackendAndroidSW * self;
    GLuint textureId;
    unsigned char * nv21;
    unsigned char * pixels;

    class Worker: public QThread
    {
    public:
        Worker(int width, int height);
        ~Worker();

        void run();
        void convert(unsigned char * source, unsigned char * target, int size);
        QMutex sync;
        bool new_image;
    private:
        int width, height;
        bool running;
        bool done;
        QMutex  mutex;
        unsigned char * target;
        unsigned char * buffer;
    };

    Worker * worker;
    unsigned char * target;

    bool inited;
    GLuint pboIds[3];
    int currentPBO;
    QElapsedTimer timer;
};

#endif // CAMERABACKENDANDROIDSW_H
