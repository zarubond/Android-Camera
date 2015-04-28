#include "camerabackendandroidsw.h"

#include <QDebug>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include "nv21torgb.cpp"


CameraBackendAndroidSW::CameraBackendAndroidSW(QObject *parent) : QObject(parent),
    java_backend(NULL)
{
    self=this;
    inited=false;
    target=NULL;

    JNINativeMethod methods[] { {"frameAvailable", "([BII)V", reinterpret_cast<void *>(CameraBackendAndroidSW::onFrameAvailable)}};

    if (QAndroidJniObject::isClassAvailable("com/zarubond/lookingglass/CameraBackendSW"))
    {
        java_backend=new QAndroidJniObject("com/zarubond/lookingglass/CameraBackendSW","()V");
        if(java_backend->isValid())
        {
            QAndroidJniEnvironment env;
            jclass objectClass = env->GetObjectClass(java_backend->object<jobject>());
            env->RegisterNatives(objectClass, methods,  sizeof(methods) / sizeof(methods[0]));
            env->DeleteLocalRef(objectClass);
        }
    }
}

CameraBackendAndroidSW::~CameraBackendAndroidSW()
{
    stop();
    delete java_backend;
}

void CameraBackendAndroidSW::start(int width, int height, GLuint textureId)
{
    pixels=new unsigned char[width*height*4];
    nv21=new unsigned char[width*height*4];
    this->textureId=textureId;
    this->width=width;
    this->height=height;
    worker=new Worker(width, height);
    worker->start();

    java_backend->callMethod<void>("startPreview", "(II)V",(jint)width, (jint)height);
    inited=false;
    timer.start();
}

void CameraBackendAndroidSW::stop()
{
    java_backend->callMethod<void>("stopPreview", "()V");
    delete [] pixels;
    delete [] nv21;
}
/*
void CameraBackendAndroidSW::updateFrame()
{
    if(!inited)
    {
        glGenBuffers(2, pboIds);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        inited=true;
    }

    static int index = 0;
    int nextIndex = 0;                  // pbo index used for next frame
    int pboMode=2;

    // "index" is used to copy pixels from a PBO to a texture object
     // "nextIndex" is used to update pixels in a PBO
     if(pboMode == 1)
     {
         // In single PBO mode, the index and nextIndex are set to 0
         index = nextIndex = 0;
     }
     else if(pboMode == 2)
     {
         // In dual PBO mode, increment current index first then get the next index
         index = (index + 1) % 2;
         nextIndex = (index + 1) % 2;
     }

     // bind the texture and PBO
     glBindTexture(GL_TEXTURE_2D, textureId);
     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);

     // copy pixels from PBO to texture object
     // Use offset instead of ponter.

     glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

     // bind PBO to update pixel values
     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);

     // map the buffer object into client's memory
     // Note that glMapBufferARB() causes sync issue.
     // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
     // for GPU to finish its job. To avoid waiting (stall), you can call
     // first glBufferDataARB() with NULL pointer before glMapBufferARB().
     // If you do that, the previous data in PBO will be discarded and
     // glMapBufferARB() returns a new allocated pointer immediately
     // even if GPU is still working with the previous data.
     glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, NULL, GL_STREAM_DRAW);

     GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, width*height*4, GL_MAP_WRITE_BIT);

     if(ptr)
     {
         memcpy(ptr, pixels, width*height*4);
         glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
     }

     // it is good idea to release PBOs with ID 0 after use.
     // Once bound with 0, all pixel operations behave normal ways.
     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
*/

void CameraBackendAndroidSW::updateFrame()
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    worker->new_image=false;
}

/*
void CameraBackendAndroidSW::updateFrame()
{
    if(!inited)
    {
        glGenBuffers(2, pboIds);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[0]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[1]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, 0, GL_STREAM_DRAW);
        //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[2]);
        //glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, 0, GL_STREAM_DRAW);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        inited=true;
        currentPBO=0;
    }

  //  glFinish();
  //  int t=timer.elapsed();

    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[currentPBO]);

   // worker->sync.lock();
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
   // worker->sync.unlock();

    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    currentPBO=(currentPBO+1)%2;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[currentPBO]);

    glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, NULL, GL_STREAM_DRAW);
    GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, width*height*4, GL_MAP_WRITE_BIT);
    target=ptr;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
   // glFinish();
   // qDebug()<<timer.elapsed()-t;


/*
    static int index = 0;
    int nextIndex = 0;                  // pbo index used for next frame
    int pboMode=2;

    // "index" is used to copy pixels from a PBO to a texture object
     // "nextIndex" is used to update pixels in a PBO
     if(pboMode == 1)
     {
         // In single PBO mode, the index and nextIndex are set to 0
         index = nextIndex = 0;
     }
     else if(pboMode == 2)
     {
         // In dual PBO mode, increment current index first then get the next index
         index = (index + 1) % 2;
         nextIndex = (index + 1) % 2;
     }

     // bind the texture and PBO
     glBindTexture(GL_TEXTURE_2D, textureId);
     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);

     glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);

     // map the buffer object into client's memory
     // Note that glMapBufferARB() causes sync issue.
     // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
     // for GPU to finish its job. To avoid waiting (stall), you can call
     // first glBufferDataARB() with NULL pointer before glMapBufferARB().
     // If you do that, the previous data in PBO will be discarded and
     // glMapBufferARB() returns a new allocated pointer immediately
     // even if GPU is still working with the previous data.
     glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4, NULL, GL_STREAM_DRAW);

    GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, width*height*4, GL_MAP_WRITE_BIT);
    target=ptr;

    if(ptr)
    {
        memcpy(ptr, pixels, width*height*4);

        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);*
}*/


void CameraBackendAndroidSW::onFrameAvailable(JNIEnv *env, jobject , jbyteArray NV21FrameData, jint width, jint height)
{
    int length= env->GetArrayLength( NV21FrameData );
    jbyte* bytes = env->GetByteArrayElements(NV21FrameData, 0 );

    self->worker->convert((unsigned char *)bytes, (unsigned char*)self->pixels, length);
 //   if(self->target!=NULL)
   //     self->worker->convert((unsigned char *)bytes, (unsigned char*)self->target, length);

    env->ReleaseByteArrayElements(NV21FrameData, bytes, JNI_ABORT);
    emit self->frameAvailable();
}

CameraBackendAndroidSW * CameraBackendAndroidSW::self=NULL;

//////////////////////////////////////////////
/// \brief CameraBackendAndroidSW::Worker::run
///
CameraBackendAndroidSW::Worker::Worker(int width, int height):
    width(width), height(height)
{
    mutex.lock();
    buffer=new unsigned char[width*height*4];
    running=true;
    done=true;
    new_image=false;
}

CameraBackendAndroidSW::Worker::~Worker()
{
    delete [] buffer;
}

void CameraBackendAndroidSW::Worker::run()
{
    while(running)
    {

        //sync.lock();
        mutex.lock();
        nv21ToRgb((const uchar*)buffer, (quint32*)target, width, height);
        done=true;
        //sync.unlock();
    }
}

void CameraBackendAndroidSW::Worker::convert(unsigned char *source, unsigned char *target, int size)
{
    if(done)
    {
        memcpy(buffer, source, size);
        this->target=target;
        done=false;
        new_image=true;
        mutex.unlock();
    }
}
